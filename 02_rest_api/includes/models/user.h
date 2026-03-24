#pragma once
#include <string>
#include <cstdint>

namespace models {

// Структура User представляет пользователя системы
// Содержит все данные пользователя: логин, имя, роль, хеш пароля
struct User {
    std::string id;              // Уникальный идентификатор пользователя (например: usr_1)
    std::string login;           // Логин для входа в систему
    std::string firstName;       // Имя пользователя
    std::string lastName;        // Фамилия пользователя
    std::string email;           // Email адрес для уведомлений
    std::string role;            // Роль: patient, doctor, admin, lab_technician
    std::string passwordHash;    // Хеш пароля (MD5 для демонстрации)
    int64_t createdAt;           // Временная метка создания (Unix timestamp)
};

// Структура UserCreateRequest используется для создания нового пользователя
// Содержит данные, которые клиент передаёт в POST запросе
struct UserCreateRequest {
    std::string login;           // Логин нового пользователя
    std::string firstName;       // Имя
    std::string lastName;        // Фамилия
    std::string email;           // Email
    std::string password;        // Пароль (будет захеширован при сохранении)
    std::string role;            // Роль пользователя (по умолчанию: patient)
};

// Структура UserLoginRequest используется для аутентификации пользователя
// Содержит логин и пароль для проверки учётных данных
struct UserLoginRequest {
    std::string login;           // Логин пользователя
    std::string password;        // Пароль для проверки
};

// Структура AuthResponse возвращается при успешной аутентификации
// Содержит JWT токен и информацию о пользователе
struct AuthResponse {
    std::string token;           // JWT токен для авторизации последующих запросов
    std::string userId;          // Идентификатор пользователя
    std::string role;            // Роль пользователя для проверки прав доступа
};

}
