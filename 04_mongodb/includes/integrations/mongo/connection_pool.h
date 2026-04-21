#pragma once

#include <Poco/MongoDB/Connection.h>
#include <Poco/MongoDB/Database.h>
#include <Poco/Logger.h>
#include <string>
#include <memory>

namespace db {

class MongoConnectionPool {
public:
    static MongoConnectionPool& instance();

    void connect(const std::string& host, int port, const std::string& database);
    Poco::MongoDB::Database& getDatabase();
    Poco::MongoDB::Connection& getConnection();
    bool isConnected() const;
    void disconnect();

private:
    MongoConnectionPool() = default;
    ~MongoConnectionPool() = default;

    MongoConnectionPool(const MongoConnectionPool&) = delete;
    MongoConnectionPool& operator=(const MongoConnectionPool&) = delete;

    std::unique_ptr<Poco::MongoDB::Connection> connection_;
    std::unique_ptr<Poco::MongoDB::Database> database_;
    bool connected_ = false;
    Poco::Logger& logger_ = Poco::Logger::get("MongoConnectionPool");
};

} // namespace db
