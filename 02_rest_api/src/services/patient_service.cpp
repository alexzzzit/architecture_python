#include "services/patient_service.h"
#include "Poco/Timestamp.h"
#include "Poco/Logger.h"

namespace services {

// Конструктор создаёт пустое хранилище пациентов
PatientService::PatientService() : _patientIdCounter(0), _recordCounter(0) {
    Poco::Logger::get("PatientService").information("PatientService initialized");
}

// Возвращает единственный экземпляр PatientService
PatientService& PatientService::getInstance() {
    static PatientService instance;
    return instance;
}

// Генерирует уникальный идентификатор для нового пациента
std::string PatientService::generatePatientId() {
    return "pat_" + std::to_string(++_patientIdCounter);
}

// Генерирует уникальный код для медицинской записи
std::string PatientService::generateRecordCode() {
    return "REC-" + std::to_string(++_recordCounter) + "-" + 
           std::to_string(Poco::Timestamp().epochTime());
}

// Регистрирует нового пациента в системе
std::optional<models::Patient> PatientService::create(const models::PatientCreateRequest& req) {
    std::lock_guard<std::mutex> lock(_mutex);

    models::Patient patient;
    patient.id = generatePatientId();
    patient.userId = req.userId;
    patient.firstName = req.firstName;
    patient.lastName = req.lastName;
    patient.middleName = req.middleName;
    patient.birthDate = req.birthDate;
    patient.phone = req.phone;
    patient.insuranceNumber = req.insuranceNumber;
    patient.createdAt = Poco::Timestamp().epochTime();

    _patients[patient.id] = patient;
    _patientRecords[patient.id] = {};

    Poco::Logger::get("PatientService").information("Patient created: %s", patient.id);
    return patient;
}

// Находит пациента по идентификатору
std::optional<models::Patient> PatientService::findById(const std::string& id) {
    std::lock_guard<std::mutex> lock(_mutex);
    
    auto it = _patients.find(id);
    if (it != _patients.end()) {
        return it->second;
    }
    return std::nullopt;
}

// Ищет пациентов по маске ФИО
std::vector<models::Patient> PatientService::searchByFullName(const std::string& firstName,
                                                               const std::string& lastName,
                                                               const std::string& middleName) {
    std::lock_guard<std::mutex> lock(_mutex);
    
    std::vector<models::Patient> result;
    for (const auto& [id, patient] : _patients) {
        bool matchFirst = firstName.empty() || 
            patient.firstName.find(firstName) != std::string::npos;
        bool matchLast = lastName.empty() || 
            patient.lastName.find(lastName) != std::string::npos;
        bool matchMiddle = middleName.empty() || 
            patient.middleName.find(middleName) != std::string::npos;
        if (matchFirst && matchLast && matchMiddle) {
            result.push_back(patient);
        }
    }
    return result;
}

// Получает все медицинские записи пациента
std::vector<models::MedicalRecord> PatientService::getPatientRecords(const std::string& patientId) {
    std::lock_guard<std::mutex> lock(_mutex);
    
    auto it = _patientRecords.find(patientId);
    if (it != _patientRecords.end()) {
        return it->second;
    }
    return {};
}

// Добавляет новую медицинскую запись к пациенту
std::optional<models::MedicalRecord> PatientService::addRecord(const models::RecordCreateRequest& req) {
    std::lock_guard<std::mutex> lock(_mutex);
    
    // Проверяем существование пациента
    if (_patients.find(req.patientId) == _patients.end()) {
        Poco::Logger::get("PatientService").warning("Patient not found: %s", req.patientId);
        return std::nullopt;
    }

    models::MedicalRecord record;
    record.code = generateRecordCode();
    record.patientId = req.patientId;
    record.doctorId = req.doctorId;
    record.diagnosis = req.diagnosis;
    record.treatment = req.treatment;
    record.notes = req.notes;
    record.createdAt = Poco::Timestamp().epochTime();
    record.updatedAt = record.createdAt;

    _patientRecords[req.patientId].push_back(record);

    Poco::Logger::get("PatientService").information("Record created: %s", record.code);
    return record;
}

// Очищает всё хранилище пациентов и записей
void PatientService::clear() {
    std::lock_guard<std::mutex> lock(_mutex);
    _patients.clear();
    _patientRecords.clear();
    _patientIdCounter = 0;
    _recordCounter = 0;
}

// Возвращает количество пациентов в хранилище
size_t PatientService::patientCount() const {
    std::lock_guard<std::mutex> lock(_mutex);
    return _patients.size();
}

// Возвращает общее количество медицинских записей
size_t PatientService::recordCount() const {
    std::lock_guard<std::mutex> lock(_mutex);
    size_t total = 0;
    for (const auto& [id, records] : _patientRecords) {
        total += records.size();
    }
    return total;
}

}
