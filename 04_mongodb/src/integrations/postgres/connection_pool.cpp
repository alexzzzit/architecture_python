#include "integrations/postgres/connection_pool.h"
#include <stdexcept>
#include <iostream>

namespace integrations::postgres {

std::unique_ptr<ConnectionPool> ConnectionPool::instance_ = nullptr;

ConnectionPool::ConnectionPool(const std::string& connStr) : connStr_(connStr) {
    for (int i = 0; i < 3; ++i) {
        PGconn* conn = PQconnectdb(connStr_.c_str());
        if (PQstatus(conn) != CONNECTION_OK) {
            std::cerr << "PostgreSQL connection failed: " << PQerrorMessage(conn) << std::endl;
            PQfinish(conn);
            throw std::runtime_error("Failed to connect to PostgreSQL");
        }
        pool_.push(std::shared_ptr<PGconn>(conn, PQfinish));
    }
}

ConnectionPool& ConnectionPool::getInstance(const std::string& connStr) {
    if (!instance_) {
        instance_ = std::unique_ptr<ConnectionPool>(new ConnectionPool(connStr));
    }
    return *instance_;
}

std::shared_ptr<PGconn> ConnectionPool::acquire() {
    std::lock_guard<std::mutex> lock(mutex_);
    if (pool_.empty()) {
        PGconn* conn = PQconnectdb(connStr_.c_str());
        if (PQstatus(conn) != CONNECTION_OK) {
            PQfinish(conn);
            throw std::runtime_error("Cannot acquire PostgreSQL connection");
        }
        return std::shared_ptr<PGconn>(conn, PQfinish);
    }
    auto conn = pool_.front();
    pool_.pop();
    return conn;
}

void ConnectionPool::release(std::shared_ptr<PGconn> conn) {
    if (conn && PQstatus(conn.get()) == CONNECTION_OK) {
        std::lock_guard<std::mutex> lock(mutex_);
        pool_.push(conn);
    }
}

PGresult* ConnectionPool::execParams(const char* query, int nParams, const char* const* paramValues) {
    auto conn = acquire();
    PGresult* res = PQexecParams(conn.get(), query, nParams, nullptr, paramValues, nullptr, nullptr, 0);
    release(conn);
    return res;
}

PGresult* ConnectionPool::exec(const char* query) {
    auto conn = acquire();
    PGresult* res = PQexec(conn.get(), query);
    release(conn);
    return res;
}

} // namespace integrations::postgres