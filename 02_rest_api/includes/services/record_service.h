#pragma once
#include "models/medical_record.h"
#include <optional>
#include <unordered_map>
#include <mutex>

namespace services {

// Класс RecordService отвечает за управление медицинскими записями
// Реализует операции создания и поиска записей по коду
// Соответствует компоненту "Medical Records Service" в архитектуре
// В текущей реализации использует in-memory хранилище для демонстрации
class RecordService {
public:
    // Возвращает единственный экземпляр RecordService (Singleton паттерн)
    static RecordService& getInstance();
    
    // Создаёт новую медицинскую запись
    // req: структура с данными медицинской записи
    // Возвращает: созданная MedicalRecord если успешно, иначе nullopt
    std::optional<models::MedicalRecord> create(const models::RecordCreateRequest& req);
    
    // Находит медицинскую запись по уникальному коду
    // code: уникальный код записи (например: REC-1001-1711123456)
    // Возвращает: MedicalRecord если найдена, иначе nullopt
    std::optional<models::MedicalRecord> findByCode(const std::string& code);
    
    // Очищает всё хранилище записей (используется в тестах)
    void clear();
    
    // Возвращает количество записей в хранилище
    // Возвращает: размер хеш-таблицы записей
    size_t count() const;

private:
    // Конструктор создаёт пустое хранилище записей
    RecordService();
    
    // Запрещаем копирование экземпляра Singleton
    RecordService(const RecordService&) = delete;
    RecordService& operator=(const RecordService&) = delete;
    
    // Генерирует уникальный код для медицинской записи
    // Возвращает: строку вида "REC-1001-1711123456"
    std::string generateRecordCode();
    
    mutable std::mutex _mutex;                              // Мьютекс для потокобезопасности
    std::unordered_map<std::string, models::MedicalRecord> _records; // Хранилище записей по коду
    int _recordCounter;                                     // Счётчик для генерации кодов
};

}
