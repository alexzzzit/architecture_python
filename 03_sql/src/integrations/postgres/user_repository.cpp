#include "integrations/postgres/user_repository.h"
#include "integrations/postgres/connection_pool.h"
#include <libpq-fe.h>
#include <iostream>

namespace integrations::postgres {

std::optional<models::User> UserRepository::create(const models::UserCreateRequest& req, const std::string& connStr) {
    const char* query = R"(
        INSERT INTO users (login, first_name, last_name, middle_name, email, password_hash, role)
        VALUES ($1, $2, $3, $4, $5, $6, $7::user_role)
        RETURNING id, login, first_name, last_name, email, role, created_at
    )";
    const char* params[7] = {
        req.login.c_str(), req.firstName.c_str(), req.lastName.c_str(),
        req.middleName.c_str(), req.email.c_str(), req.passwordHash.c_str(), req.role.c_str()
    };
    
    PGresult* res = ConnectionPool::getInstance(connStr).execParams(query, 7, params);
    if (!res || PQresultStatus(res) != PGRES_TUPLES_OK) {
        if (res) PQclear(res);
        return std::nullopt;
    }
    
    models::User user;
    user.id = PQgetvalue(res, 0, 0);
    user.login = PQgetvalue(res, 0, 1);
    user.firstName = PQgetvalue(res, 0, 2);
    user.lastName = PQgetvalue(res, 0, 3);
    user.middleName = PQgetvalue(res, 0, 4) ? PQgetvalue(res, 0, 4) : "";
    user.email = PQgetvalue(res, 0, 5);
    user.role = PQgetvalue(res, 0, 6);
    user.createdAt = std::time(nullptr);
    
    PQclear(res);
    return user;
}

std::optional<models::User> UserRepository::findByLogin(const std::string& login, const std::string& connStr) {
    const char* query = "SELECT id, login, first_name, last_name, email, role, created_at FROM users WHERE login = $1 AND is_active = true";
    const char* params[1] = { login.c_str() };
    
    PGresult* res = ConnectionPool::getInstance(connStr).execParams(query, 1, params);
    if (!res || PQresultStatus(res) != PGRES_TUPLES_OK || PQntuples(res) == 0) {
        if (res) PQclear(res);
        return std::nullopt;
    }
    
    models::User user;
    user.id = PQgetvalue(res, 0, 0);
    user.login = PQgetvalue(res, 0, 1);
    user.firstName = PQgetvalue(res, 0, 2);
    user.lastName = PQgetvalue(res, 0, 3);
    user.email = PQgetvalue(res, 0, 4);
    user.role = PQgetvalue(res, 0, 5);
    user.createdAt = std::time(nullptr);
    
    PQclear(res);
    return user;
}

std::vector<models::User> UserRepository::searchByName(const std::string& firstName, const std::string& lastName, const std::string& connStr) {
    std::vector<models::User> results;
    std::string query = "SELECT id, login, first_name, last_name, email, role FROM users WHERE is_active = true";
    
    std::vector<std::string> conditions;
    std::vector<const char*> params;
    int paramIdx = 1;
    
    if (!firstName.empty()) {
        conditions.push_back("first_name ILIKE $" + std::to_string(paramIdx++));
        params.push_back(("%" + firstName + "%").c_str());
    }
    if (!lastName.empty()) {
        conditions.push_back("last_name ILIKE $" + std::to_string(paramIdx++));
        params.push_back(("%" + lastName + "%").c_str());
    }
    
    if (!conditions.empty()) {
        query += " AND (";
        for (size_t i = 0; i < conditions.size(); ++i) {
            if (i > 0) query += " OR ";
            query += conditions[i];
        }
        query += ")";
    }
    query += " ORDER BY last_name, first_name LIMIT 50";
    
    PGresult* res = ConnectionPool::getInstance(connStr).execParams(query.c_str(), params.size(), params.data());
    
    if (res && PQresultStatus(res) == PGRES_TUPLES_OK) {
        for (int i = 0; i < PQntuples(res); ++i) {
            models::User u;
            u.id = PQgetvalue(res, i, 0);
            u.login = PQgetvalue(res, i, 1);
            u.firstName = PQgetvalue(res, i, 2);
            u.lastName = PQgetvalue(res, i, 3);
            u.email = PQgetvalue(res, i, 4);
            u.role = PQgetvalue(res, i, 5);
            results.push_back(u);
        }
    }
    if (res) PQclear(res);
    return results;
}

bool UserRepository::validatePassword(const models::User& user, const std::string& password) {
    // В продакшене: сравнение хэша через bcrypt/argon2
    // Для демонстрации — простая проверка
    return !password.empty() && !user.passwordHash.empty();
}

std::optional<models::User> UserRepository::findById(const std::string& id, const std::string& connStr) {
    const char* query = "SELECT id, login, first_name, last_name, email, role FROM users WHERE id = $1 AND is_active = true";
    const char* params[1] = { id.c_str() };
    
    PGresult* res = ConnectionPool::getInstance(connStr).execParams(query, 1, params);
    if (!res || PQresultStatus(res) != PGRES_TUPLES_OK || PQntuples(res) == 0) {
        if (res) PQclear(res);
        return std::nullopt;
    }
    
    models::User user;
    user.id = PQgetvalue(res, 0, 0);
    user.login = PQgetvalue(res, 0, 1);
    user.firstName = PQgetvalue(res, 0, 2);
    user.lastName = PQgetvalue(res, 0, 3);
    user.email = PQgetvalue(res, 0, 4);
    user.role = PQgetvalue(res, 0, 5);
    
    PQclear(res);
    return user;
}

} // namespace integrations::postgres