#pragma once

#include <string>
#include <Poco/Timestamp.h>
#include <Poco/MongoDB/Document.h>

namespace models {

class User {
public:
    enum class Role {
        PATIENT,
        DOCTOR,
        ADMIN,
        LAB_TECHNICIAN
    };

    User() = default;

    long long id = 0;
    std::string login;
    std::string email;
    std::string passwordHash;
    Role role = Role::PATIENT;
    bool isActive = true;
    Poco::Timestamp createdAt;
    Poco::Timestamp updatedAt;

    Poco::MongoDB::Document toBson() const;
    static User fromBson(const Poco::MongoDB::Document& doc);

    static std::string roleToString(Role role);
    static Role roleFromString(const std::string& role);
};

} // namespace models