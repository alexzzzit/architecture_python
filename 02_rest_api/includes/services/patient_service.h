#pragma once
#include "models/patient.h"
#include "models/medical_record.h"
#include <vector>
#include <optional>
#include <unordered_map>
#include <mutex>

namespace services {

// Класс PatientService отвечает за управление пациентами и их медицинскими записями
// Реализует CRUD операции для пациентов и записей
// Соответствует компоненту "Patient Service" в архитектуре
// В текущей реализации использует in-memory хранилище для демонстрации
class PatientService {
public:
    // Возвращает единственный экземпляр PatientService (Singleton паттерн)
    static PatientService& getInstance();
    
    // Регистрирует нового пациента в системе
    // req: структура с данными пациента для регистрации
    // Возвращает: созданный Patient если успешно, иначе nullopt
    std::optional<models::Patient> create(const models::PatientCreateRequest& req);
    
    // Находит пациента по идентификатору
    // id: уникальный идентификатор пациента
    // Возвращает: Patient если найден, иначе nullopt
    std::optional<models::Patient> findById(const std::string& id);
    
    // Ищет пациентов по маске ФИО
    // firstName: маска для поиска по имени (может быть пустой)
    // lastName: маска для поиска по фамилии (может быть пустой)
    // middleName: маска для поиска по отчеству (может быть пустой)
    // Возвращает: вектор найденных пациентов (может быть пустым)
    std::vector<models::Patient> searchByFullName(const std::string& firstName, 
                                                   const std::string& lastName,
                                                   const std::string& middleName);
    
    // Получает все медицинские записи пациента
    // patientId: идентификатор пациента
    // Возвращает: вектор медицинских записей (может быть пустым)
    std::vector<models::MedicalRecord> getPatientRecords(const std::string& patientId);
    
    // Добавляет новую медицинскую запись к пациенту
    // req: структура с данными медицинской записи
    // Возвращает: созданная MedicalRecord если успешно, иначе nullopt
    std::optional<models::MedicalRecord> addRecord(const models::RecordCreateRequest& req);
    
    // Очищает всё хранилище пациентов и записей (используется в тестах)
    void clear();
    
    // Возвращает количество пациентов в хранилище
    // Возвращает: размер хеш-таблицы пациентов
    size_t patientCount() const;
    
    // Возвращает общее количество медицинских записей
    // Возвращает: суммарное количество записей по всем пациентам
    size_t recordCount() const;

private:
    // Конструктор создаёт пустое хранилище пациентов
    PatientService();
    
    // Запрещаем копирование экземпляра Singleton
    PatientService(const PatientService&) = delete;
    PatientService& operator=(const PatientService&) = delete;
    
    // Генерирует уникальный идентификатор для нового пациента
    // Возвращает: строку вида "pat_1", "pat_2", etc.
    std::string generatePatientId();
    
    // Генерирует уникальный код для медицинской записи
    // Возвращает: строку вида "REC-1001-1711123456"
    std::string generateRecordCode();
    
    mutable std::mutex _mutex;                              // Мьютекс для потокобезопасности
    std::unordered_map<std::string, models::Patient> _patients; // Хранилище пациентов по ID
    std::unordered_map<std::string, std::vector<models::MedicalRecord>> _patientRecords; // Записи по пациенту
    int _patientIdCounter;                                  // Счётчик для генерации ID пациентов
    int _recordCounter;                                     // Счётчик для генерации кодов записей
};

}
