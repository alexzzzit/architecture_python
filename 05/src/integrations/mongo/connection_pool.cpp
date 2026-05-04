#include "integrations/mongo/connection_pool.h"
#include <Poco/Exception.h>

namespace db {

MongoConnectionPool& MongoConnectionPool::instance() {
    static MongoConnectionPool instance;
    return instance;
}

void MongoConnectionPool::connect(const std::string& host, int port, const std::string& database) {
    if (connected_) {
        return;
    }

    try {
        connection_ = std::make_unique<Poco::MongoDB::Connection>(host, port);
        database_ = std::make_unique<Poco::MongoDB::Database>(database);
        database_->setConnection(*connection_);

        connected_ = true;
        logger_.information("Connected to MongoDB at %s:%d, database: %s", host, port, database);
    } catch (const Poco::Exception& ex) {
        logger_.error("Failed to connect to MongoDB: %s", ex.displayText());
        throw;
    }
}

Poco::MongoDB::Database& MongoConnectionPool::getDatabase() {
    if (!connected_ || !database_) {
        throw Poco::IllegalStateException("MongoDB not connected");
    }
    return *database_;
}

Poco::MongoDB::Connection& MongoConnectionPool::getConnection() {
    if (!connected_ || !connection_) {
        throw Poco::IllegalStateException("MongoDB not connected");
    }
    return *connection_;
}

bool MongoConnectionPool::isConnected() const {
    return connected_;
}

void MongoConnectionPool::disconnect() {
    if (connected_) {
        connection_.reset();
        database_.reset();
        connected_ = false;
        logger_.information("Disconnected from MongoDB");
    }
}

} // namespace db
