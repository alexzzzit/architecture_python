#pragma once

#include <string>
#include <unordered_map>
#include <mutex>
#include <Poco/Timestamp.h>
#include <Poco/Logger.h>

namespace cache {

struct CacheEntry {
    std::string value;
    Poco::Timestamp expiresAt;

    bool isExpired() const {
        return Poco::Timestamp() > expiresAt;
    }
};

class CacheService {
public:
    explicit CacheService(const std::string& prefix = "med:")
        : prefix_(prefix)
    {
        Poco::Logger::get("Cache").information(
            "In-memory cache initialized (prefix: %s)",
            prefix_.c_str()
        );
    }

    std::string get(const std::string& key)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        std::string full_key = prefix_ + key;
        auto it = cache_.find(full_key);

        if (it == cache_.end()) {
            Poco::Logger::get("Cache").debug("CACHE_MISS: %s", full_key.c_str());
            return "";
        }

        if (it->second.isExpired()) {
            cache_.erase(it);
            Poco::Logger::get("Cache").debug("CACHE_EXPIRED: %s", full_key.c_str());
            return "";
        }

        Poco::Logger::get("Cache").debug("CACHE_HIT: %s", full_key.c_str());
        return it->second.value;
    }

    void set(const std::string& key, const std::string& value, int ttlSeconds)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        CacheEntry entry;
        entry.value = value;
        int64_t ttl_microseconds = static_cast<int64_t>(ttlSeconds) * 1000000LL;
        entry.expiresAt = Poco::Timestamp() + ttl_microseconds;
        std::string full_key = prefix_ + key;
        cache_[full_key] = entry;
    }

    void del(const std::string& key)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        cache_.erase(prefix_ + key);
    }

    void invalidatePattern(const std::string& pattern)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        std::string full_pattern = prefix_ + pattern;
        for (auto it = cache_.begin(); it != cache_.end(); ) {
            if (it->first.find(full_pattern) == 0) {
                it = cache_.erase(it);
            } else {
                ++it;
            }
        }
    }

private:
    std::unordered_map<std::string, CacheEntry> cache_;
    mutable std::mutex mutex_;
    std::string prefix_;
};

} // namespace cache