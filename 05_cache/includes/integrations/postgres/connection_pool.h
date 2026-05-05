#pragma once
#include <libpq-fe.h>
#include <memory>
#include <string>
#include <mutex>
#include <queue>

namespace integrations::postgres {

class ConnectionPool {
public:
    static ConnectionPool& getInstance(const std::string& connStr);
    std::shared_ptr<PGconn> acquire();
    void release(std::shared_ptr<PGconn> conn);
    PGresult* execParams(const char* query, int nParams, const char* const* paramValues);
    PGresult* exec(const char* query);
    
private:
    ConnectionPool(const std::string& connStr);
    std::string connStr_;
    std::queue<std::shared_ptr<PGconn>> pool_;
    mutable std::mutex mutex_;
    static std::unique_ptr<ConnectionPool> instance_;
};

} // namespace integrations::postgres