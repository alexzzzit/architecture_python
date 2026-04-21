#pragma once
#include "models/user.h"
#include "models/requests.h"
#include <optional>
#include <vector>
#include <string>

namespace services {

class UserService {
public:
    static UserService& getInstance();
    
    std::optional<models::User> create(const models::UserCreateRequest& req);
    std::optional<models::User> findByLogin(const std::string& login);
    std::vector<models::User> searchByName(const std::string& firstName, const std::string& lastName);
    bool validatePassword(const models::User& user, const std::string& password);
    void clear();
    size_t count() const;
    
private:
    UserService() = default;
    static UserService* instance_;
};

} // namespace services