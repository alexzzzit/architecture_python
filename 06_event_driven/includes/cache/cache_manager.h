#pragma once

#include <string>
#include <unordered_map>
#include <mutex>
#include <Poco/Timestamp.h>
#include <Poco/Logger.h>

namespace cache {

class CacheEntry {
public:
    CacheEntry() = default;
    
    CacheEntry(const std::string& value, Poco::Timestamp::TimeDiff ttlSeconds)
        : value_(value)
        , createdAt_(Poco::Timestamp())
        , ttlSeconds_(ttlSeconds)
    {}

    const std::string& value() const { return value_; }
    
    bool isExpired() const {
        Poco::Timestamp now;
        Poco::Timestamp::TimeDiff elapsed = (now - createdAt_) / 1000000;
        return ttlSeconds_ > 0 && elapsed > ttlSeconds_;
    }

private:
    std::string value_;
    Poco::Timestamp createdAt_;
    Poco::Timestamp::TimeDiff ttlSeconds_;
};

class CacheManager {
public:
    explicit CacheManager(Poco::Timestamp::TimeDiff defaultTtlSeconds = 300)
        : defaultTtlSeconds_(defaultTtlSeconds)
    {}

    void set(const std::string& key, const std::string& value, 
             Poco::Timestamp::TimeDiff ttlSeconds = -1) {
        std::lock_guard<std::mutex> lock(mutex_);
        if (ttlSeconds < 0) ttlSeconds = defaultTtlSeconds_;
        cache_.emplace(key, CacheEntry(value, ttlSeconds));
        Poco::Logger::get("CacheManager").debug(Poco::format("Cache set: %s", key));
    }

    bool get(const std::string& key, std::string& outValue) {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = cache_.find(key);
        if (it == cache_.end()) {
            Poco::Logger::get("CacheManager").debug(Poco::format("Cache miss: %s", key));
            return false;
        }
        if (it->second.isExpired()) {
            cache_.erase(it);
            Poco::Logger::get("CacheManager").debug(Poco::format("Cache expired: %s", key));
            return false;
        }
        outValue = it->second.value();
        Poco::Logger::get("CacheManager").debug(Poco::format("Cache hit: %s", key));
        return true;
    }

    void remove(const std::string& key) {
        std::lock_guard<std::mutex> lock(mutex_);
        cache_.erase(key);
        Poco::Logger::get("CacheManager").debug(Poco::format("Cache remove: %s", key));
    }

    void clear() {
        std::lock_guard<std::mutex> lock(mutex_);
        cache_.clear();
        Poco::Logger::get("CacheManager").information("Cache cleared");
    }

    size_t size() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return cache_.size();
    }

private:
    mutable std::mutex mutex_;
    std::unordered_map<std::string, CacheEntry> cache_;
    Poco::Timestamp::TimeDiff defaultTtlSeconds_;
};

}