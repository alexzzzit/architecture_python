#include "services/record_service.h"
#include "Poco/Timestamp.h"
#include "Poco/Logger.h"

namespace services {

// Конструктор создаёт пустое хранилище записей
RecordService::RecordService() : _recordCounter(1000) {
    Poco::Logger::get("RecordService").information("RecordService initialized");
}

// Возвращает единственный экземпляр RecordService
RecordService& RecordService::getInstance() {
    static RecordService instance;
    return instance;
}

// Генерирует уникальный код для медицинской записи
std::string RecordService::generateRecordCode() {
    return "REC-" + std::to_string(++_recordCounter) + "-" + 
           std::to_string(Poco::Timestamp().epochTime());
}

// Создаёт новую медицинскую запись
std::optional<models::MedicalRecord> RecordService::create(const models::RecordCreateRequest& req) {
    std::lock_guard<std::mutex> lock(_mutex);
    
    models::MedicalRecord record;
    record.code = generateRecordCode();
    record.patientId = req.patientId;
    record.doctorId = req.doctorId;
    record.diagnosis = req.diagnosis;
    record.treatment = req.treatment;
    record.notes = req.notes;
    record.createdAt = Poco::Timestamp().epochTime();
    record.updatedAt = record.createdAt;

    _records[record.code] = record;
    
    Poco::Logger::get("RecordService").information("Record created: %s", record.code);
    return record;
}

// Находит медицинскую запись по уникальному коду
std::optional<models::MedicalRecord> RecordService::findByCode(const std::string& code) {
    std::lock_guard<std::mutex> lock(_mutex);
    
    auto it = _records.find(code);
    if (it != _records.end()) {
        return it->second;
    }
    return std::nullopt;
}

// Очищает всё хранилище записей
void RecordService::clear() {
    std::lock_guard<std::mutex> lock(_mutex);
    _records.clear();
    _recordCounter = 1000;
}

// Возвращает количество записей в хранилище
size_t RecordService::count() const {
    std::lock_guard<std::mutex> lock(_mutex);
    return _records.size();
}

}
