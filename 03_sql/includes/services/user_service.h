#pragma once
#include "models/user.h"
#include <vector>
#include <optional>
#include <unordered_map>
#include <mutex>

namespace services {

// Класс UserService отвечает за управление пользователями системы
// Реализует CRUD операции: создание, чтение, поиск пользователей
// Соответствует компоненту "User Service" в архитектуре
// В текущей реализации использует in-memory хранилище для демонстрации
class UserService {
public:
    // Возвращает единственный экземпляр UserService (Singleton паттерн)
    static UserService& getInstance();
    
    // Создаёт нового пользователя в системе
    // req: структура с данными пользователя для создания
    // Возвращает: созданный User если успешно, иначе nullopt (например, логин занят)
    std::optional<models::User> create(const models::UserCreateRequest& req);
    
    // Находит пользователя по логину
    // login: логин пользователя для поиска
    // Возвращает: User если найден, иначе nullopt
    std::optional<models::User> findByLogin(const std::string& login);
    
    // Находит пользователя по идентификатору
    // id: уникальный идентификатор пользователя
    // Возвращает: User если найден, иначе nullopt
    std::optional<models::User> findById(const std::string& id);
    
    // Ищет пользователей по маске имени и фамилии
    // firstName: маска для поиска по имени (может быть пустой)
    // lastName: маска для поиска по фамилии (может быть пустой)
    // Возвращает: вектор найденных пользователей (может быть пустым)
    std::vector<models::User> searchByName(const std::string& firstName, 
                                           const std::string& lastName);
    
    // Проверяет правильность пароля пользователя
    // user: пользователь для проверки
    // password: пароль в открытом виде для сравнения
    // Возвращает: true если хеш пароля совпадает
    bool validatePassword(const models::User& user, const std::string& password);
    
    // Очищает всё хранилище пользователей (используется в тестах)
    void clear();
    
    // Возвращает количество пользователей в хранилище
    // Возвращает: размер хеш-таблицы пользователей
    size_t count() const;

private:
    // Конструктор создаёт пустое хранилище пользователей
    UserService();
    
    // Запрещаем копирование экземпляра Singleton
    UserService(const UserService&) = delete;
    UserService& operator=(const UserService&) = delete;
    
    // Генерирует уникальный идентификатор для нового пользователя
    // Возвращает: строку вида "usr_1", "usr_2", etc.
    std::string generateId();
    
    // Создаёт хеш пароля для безопасного хранения
    // password: пароль в открытом виде
    // Возвращает: MD5 хеш пароля
    std::string hashPassword(const std::string& password);
    
    mutable std::mutex _mutex;                              // Мьютекс для потокобезопасности
    std::unordered_map<std::string, models::User> _users;   // Хранилище пользователей по ID
    std::unordered_map<std::string, std::string> _loginIndex; // Индекс для быстрого поиска по логину
    int _userIdCounter;                                     // Счётчик для генерации ID
};

}
