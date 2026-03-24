#pragma once
#include <string>
#include <cstdint>

namespace models {

// Структура MedicalRecord представляет медицинскую запись (диагноз, лечение)
// Содержит всю информацию о визите пациента к врачу
struct MedicalRecord {
    std::string code;            // Уникальный код записи (например: REC-1001-1711123456)
    std::string patientId;       // Идентификатор пациента, к которому относится запись
    std::string doctorId;        // Идентификатор врача, создавшего запись
    std::string diagnosis;       // Диагноз пациента
    std::string treatment;       // Назначенное лечение
    std::string notes;           // Дополнительные заметки врача
    int64_t createdAt;           // Временная метка создания (Unix timestamp)
    int64_t updatedAt;           // Временная метка последнего обновления
};

// Структура RecordCreateRequest используется для создания новой медицинской записи
// Содержит данные, которые врач передаёт при создании записи
struct RecordCreateRequest {
    std::string patientId;       // Идентификатор пациента
    std::string doctorId;        // Идентификатор врача (берётся из JWT токена)
    std::string diagnosis;       // Диагноз
    std::string treatment;       // Лечение
    std::string notes;           // Заметки (опционально)
};

}
