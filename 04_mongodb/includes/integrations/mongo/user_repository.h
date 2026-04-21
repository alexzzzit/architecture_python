#pragma once

#include <Poco/MongoDB/Database.h>
#include <Poco/MongoDB/Document.h>
#include <optional>
#include <vector>
#include "models/user.h"

namespace db {

class UserRepository {
public:
    explicit UserRepository(Poco::MongoDB::Database& database);

    bool create(const models::User& user);
    std::optional<models::User> findById(long long id);
    std::optional<models::User> findByLogin(const std::string& login);
    std::optional<models::User> findByEmail(const std::string& email);
    std::vector<models::User> findByNamePattern(const std::string& firstName, const std::string& lastName);
    std::vector<models::User> findAll();
    bool update(long long id, const models::User& user);
    bool deactivate(long long id);
    bool remove(long long id);

private:
    Poco::MongoDB::Document toDocument(const models::User& user) const;
    models::User fromDocument(const Poco::MongoDB::Document& doc) const;

    Poco::MongoDB::Database& database_;
};

} // namespace db