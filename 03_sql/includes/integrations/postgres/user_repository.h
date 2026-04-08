#pragma once
#include "models/user.h"
#include <optional>
#include <vector>
#include <string>

namespace integrations::postgres {

class UserRepository {
public:
    static std::optional<models::User> create(const models::UserCreateRequest& req, const std::string& connStr);
    static std::optional<models::User> findByLogin(const std::string& login, const std::string& connStr);
    static std::vector<models::User> searchByName(const std::string& firstName, const std::string& lastName, const std::string& connStr);
    static bool validatePassword(const models::User& user, const std::string& password);
    static std::optional<models::User> findById(const std::string& id, const std::string& connStr);
};

} // namespace integrations::postgres