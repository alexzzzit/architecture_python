#pragma once
#include <string>
#include <ctime>

namespace models {

struct User {
    std::string id;
    std::string login;
    std::string firstName;
    std::string lastName;
    std::string middleName;
    std::string email;
    std::string passwordHash;
    std::string role;
    bool isActive = true;
    time_t createdAt = 0;
};

struct UserCreateRequest {
    std::string login;
    std::string firstName;
    std::string lastName;
    std::string middleName;
    std::string email;
    std::string password;
    std::string passwordHash;
    std::string role;
};

} // namespace models