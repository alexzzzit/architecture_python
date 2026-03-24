#pragma once
#include <string>
#include <cstdint>

namespace models {

// Структура Patient представляет пациента медицинской системы
// Содержит персональные данные и информацию о страховке
struct Patient {
    std::string id;              // Уникальный идентификатор пациента (например: pat_1)
    std::string userId;          // Связь с пользователем (user.id)
    std::string firstName;       // Имя пациента
    std::string lastName;        // Фамилия пациента
    std::string middleName;      // Отчество пациента
    std::string birthDate;       // Дата рождения (формат: YYYY-MM-DD)
    std::string phone;           // Контактный телефон
    std::string insuranceNumber; // Номер страхового полиса
    int64_t createdAt;           // Временная метка регистрации (Unix timestamp)
};

// Структура PatientCreateRequest используется для регистрации нового пациента
// Содержит данные, которые передаются в POST запросе при регистрации
struct PatientCreateRequest {
    std::string userId;          // Идентификатор связанного пользователя
    std::string firstName;       // Имя пациента
    std::string lastName;        // Фамилия пациента
    std::string middleName;      // Отчество пациента
    std::string birthDate;       // Дата рождения
    std::string phone;           // Телефон для связи
    std::string insuranceNumber; // Номер страховки
};

}
