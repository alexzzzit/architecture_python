#include "services/user_service.h"
#include "Poco/Timestamp.h"
#include "Poco/MD5Engine.h"
#include "Poco/DigestStream.h"
#include "Poco/Logger.h"
#include <sstream>

namespace services {

// Конструктор создаёт пустое хранилище пользователей
UserService::UserService() : _userIdCounter(0) {
    Poco::Logger::get("UserService").information("UserService initialized");
}

// Возвращает единственный экземпляр UserService
UserService& UserService::getInstance() {
    static UserService instance;
    return instance;
}

// Генерирует уникальный идентификатор для нового пользователя
// Возвращает: строку вида "usr_1", "usr_2", etc.
std::string UserService::generateId() {
    std::lock_guard<std::mutex> lock(_mutex);
    return "usr_" + std::to_string(++_userIdCounter);
}

// Создаёт хеш пароля для безопасного хранения
// password: пароль в открытом виде
// Возвращает: MD5 хеш пароля
std::string UserService::hashPassword(const std::string& password) {
    Poco::MD5Engine md5;
    Poco::DigestOutputStream ds(md5);
    ds << password;
    ds.close();
    return Poco::DigestEngine::digestToHex(md5.digest());
}

// Создаёт нового пользователя в системе
// req: структура с данными пользователя для создания
// Возвращает: созданный User если успешно, иначе nullopt (логин занят)
std::optional<models::User> UserService::create(const models::UserCreateRequest& req) {
    std::lock_guard<std::mutex> lock(_mutex);
    
    // Проверяем уникальность логина
    if (_loginIndex.find(req.login) != _loginIndex.end()) {
        Poco::Logger::get("UserService").warning("User creation failed: login exists - %s", req.login);
        return std::nullopt;
    }

    // Создаём нового пользователя
    models::User user;
    user.id = "usr_" + std::to_string(++_userIdCounter);
    user.login = req.login;
    user.firstName = req.firstName;
    user.lastName = req.lastName;
    user.email = req.email;
    user.role = req.role;
    user.passwordHash = hashPassword(req.password);
    user.createdAt = Poco::Timestamp().epochTime();

    // Сохраняем в хранилище
    _users[user.id] = user;
    _loginIndex[user.login] = user.id;

    Poco::Logger::get("UserService").information("User created: %s", user.id);
    return user;
}

// Находит пользователя по логину
// login: логин пользователя для поиска
// Возвращает: User если найден, иначе nullopt
std::optional<models::User> UserService::findByLogin(const std::string& login) {
    std::lock_guard<std::mutex> lock(_mutex);
    
    auto it = _loginIndex.find(login);
    if (it != _loginIndex.end()) {
        return _users.at(it->second);
    }
    return std::nullopt;
}

// Находит пользователя по идентификатору
std::optional<models::User> UserService::findById(const std::string& id) {
    std::lock_guard<std::mutex> lock(_mutex);
    
    auto it = _users.find(id);
    if (it != _users.end()) {
        return it->second;
    }
    return std::nullopt;
}

// Ищет пользователей по маске имени и фамилии
std::vector<models::User> UserService::searchByName(const std::string& firstName, 
                                                     const std::string& lastName) {
    std::lock_guard<std::mutex> lock(_mutex);
    
    std::vector<models::User> result;
    for (const auto& [id, user] : _users) {
        // Проверяем совпадение по имени и фамилии
        bool matchFirst = firstName.empty() || 
            user.firstName.find(firstName) != std::string::npos;
        bool matchLast = lastName.empty() || 
            user.lastName.find(lastName) != std::string::npos;
        if (matchFirst && matchLast) {
            result.push_back(user);
        }
    }
    return result;
}

// Проверяет правильность пароля пользователя
bool UserService::validatePassword(const models::User& user, const std::string& password) {
    return user.passwordHash == hashPassword(password);
}

// Очищает всё хранилище пользователей (для тестов)
void UserService::clear() {
    std::lock_guard<std::mutex> lock(_mutex);
    _users.clear();
    _loginIndex.clear();
    _userIdCounter = 0;
}

// Возвращает количество пользователей в хранилище
size_t UserService::count() const {
    std::lock_guard<std::mutex> lock(_mutex);
    return _users.size();
}

}
