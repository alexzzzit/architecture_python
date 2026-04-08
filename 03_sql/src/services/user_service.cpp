#include "services/user_service.h"
#include "integrations/postgres/user_repository.h"
#include <Poco/Environment.h>

namespace services {

UserService* UserService::instance_ = nullptr;

UserService& UserService::getInstance() {
    if (!instance_) {
        instance_ = new UserService();
    }
    return *instance_;
}

std::optional<models::User> UserService::create(const models::UserCreateRequest& req) {
    std::string dbUrl = Poco::Environment::get("DATABASE_URL", "postgresql://medical_user:medical_password_2025@localhost:5432/medical_records");
    return integrations::postgres::UserRepository::create(req, dbUrl);
}

std::optional<models::User> UserService::findByLogin(const std::string& login) {
    std::string dbUrl = Poco::Environment::get("DATABASE_URL", "postgresql://medical_user:medical_password_2025@localhost:5432/medical_records");
    return integrations::postgres::UserRepository::findByLogin(login, dbUrl);
}

std::vector<models::User> UserService::searchByName(const std::string& firstName, const std::string& lastName) {
    std::string dbUrl = Poco::Environment::get("DATABASE_URL", "postgresql://medical_user:medical_password_2025@localhost:5432/medical_records");
    return integrations::postgres::UserRepository::searchByName(firstName, lastName, dbUrl);
}

bool UserService::validatePassword(const models::User& user, const std::string& password) {
    return user.passwordHash == password;
}

void UserService::clear() {}
size_t UserService::count() const { return 0; }

} // namespace services