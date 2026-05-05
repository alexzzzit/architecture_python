#include "integrations/postgres/user_repository.h"
#include <libpq-fe.h>
#include <cstring>  // 🔥 Для strcmp
#include <stdexcept>

namespace integrations {
namespace postgres {

std::optional<models::User> UserRepository::create(const models::UserCreateRequest& req, const std::string& dbUrl) {
    PGconn* conn = PQconnectdb(dbUrl.c_str());
    if (PQstatus(conn) != CONNECTION_OK) {
        PQfinish(conn);
        throw std::runtime_error("Failed to connect to PostgreSQL");
    }

    const char* sql = "INSERT INTO users (login, first_name, last_name, middle_name, email, password_hash, role) "
                      "VALUES ($1, $2, $3, $4, $5, $6, $7::user_role) "
                      "RETURNING id, login, first_name, last_name, middle_name, email, role, is_active, created_at";
    
    const char* params[7] = {
        req.login.c_str(), req.firstName.c_str(), req.lastName.c_str(),
        req.middleName.c_str(), req.email.c_str(), req.password.c_str(), req.role.c_str()
    };
    
    PGresult* res = PQexecParams(conn, sql, 7, nullptr, params, nullptr, nullptr, 0);
    
    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        PQclear(res);
        PQfinish(conn);
        return std::nullopt;
    }
    
    models::User user;
    user.id = PQgetvalue(res, 0, 0);
    user.login = PQgetvalue(res, 0, 1);
    user.firstName = PQgetvalue(res, 0, 2);
    user.lastName = PQgetvalue(res, 0, 3);
    user.middleName = PQgetvalue(res, 0, 4);
    user.email = PQgetvalue(res, 0, 5);
    user.role = PQgetvalue(res, 0, 6);
    user.isActive = strcmp(PQgetvalue(res, 0, 7), "t") == 0;
    user.createdAt = std::time(nullptr);
    
    PQclear(res);
    PQfinish(conn);
    return user;
}

std::optional<models::User> UserRepository::findByLogin(const std::string& login, const std::string& dbUrl) {
    PGconn* conn = PQconnectdb(dbUrl.c_str());
    if (PQstatus(conn) != CONNECTION_OK) {
        PQfinish(conn);
        throw std::runtime_error("Failed to connect to PostgreSQL");
    }

    const char* sql = "SELECT id, login, first_name, last_name, middle_name, email, password_hash, role, is_active, created_at "
                      "FROM users WHERE login = $1 AND is_active = TRUE";
    
    const char* params[1] = { login.c_str() };
    
    PGresult* res = PQexecParams(conn, sql, 1, nullptr, params, nullptr, nullptr, 0);
    
    if (PQresultStatus(res) != PGRES_TUPLES_OK || PQntuples(res) == 0) {
        PQclear(res);
        PQfinish(conn);
        return std::nullopt;
    }
    
    models::User user;
    user.id = PQgetvalue(res, 0, 0);
    user.login = PQgetvalue(res, 0, 1);
    user.firstName = PQgetvalue(res, 0, 2);
    user.lastName = PQgetvalue(res, 0, 3);
    user.middleName = PQgetvalue(res, 0, 4);
    user.email = PQgetvalue(res, 0, 5);
    user.passwordHash = PQgetvalue(res, 0, 6);
    user.role = PQgetvalue(res, 0, 7);
    user.isActive = strcmp(PQgetvalue(res, 0, 8), "t") == 0;
    user.createdAt = std::time(nullptr);
    
    PQclear(res);
    PQfinish(conn);
    return user;
}

std::vector<models::User> UserRepository::searchByName(const std::string& firstName, const std::string& lastName, const std::string& dbUrl) {
    PGconn* conn = PQconnectdb(dbUrl.c_str());
    if (PQstatus(conn) != CONNECTION_OK) {
        PQfinish(conn);
        throw std::runtime_error("Failed to connect to PostgreSQL");
    }

    std::string sql = "SELECT id, login, first_name, last_name, middle_name, email, role, is_active, created_at "
                      "FROM users WHERE is_active = TRUE";
    
    if (!firstName.empty() || !lastName.empty()) {
        sql += " AND (";
        bool first = true;
        if (!firstName.empty()) {
            sql += "first_name ILIKE '%" + firstName + "%'";
            first = false;
        }
        if (!lastName.empty()) {
            if (!first) sql += " OR ";
            sql += "last_name ILIKE '%" + lastName + "%'";
        }
        sql += ")";
    }
    sql += " ORDER BY last_name, first_name LIMIT 50";
    
    PGresult* res = PQexec(conn, sql.c_str());
    std::vector<models::User> users;
    
    if (PQresultStatus(res) == PGRES_TUPLES_OK) {
        for (int i = 0; i < PQntuples(res); ++i) {
            models::User user;
            user.id = PQgetvalue(res, i, 0);
            user.login = PQgetvalue(res, i, 1);
            user.firstName = PQgetvalue(res, i, 2);
            user.lastName = PQgetvalue(res, i, 3);
            user.middleName = PQgetvalue(res, i, 4);
            user.email = PQgetvalue(res, i, 5);
            user.role = PQgetvalue(res, i, 6);
            user.isActive = strcmp(PQgetvalue(res, i, 7), "t") == 0;
            user.createdAt = std::time(nullptr);
            users.push_back(user);
        }
    }
    
    PQclear(res);
    PQfinish(conn);
    return users;
}

}} // namespace integrations::postgres