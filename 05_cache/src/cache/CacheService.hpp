// src/cache/CacheService.hpp — без Poco::Optional
#pragma once
#include <Poco/Timestamp.h>
#include <Poco/Logger.h>
#include <string>
#include <unordered_map>
#include <mutex>

namespace cache {

struct CacheEntry {
    std::string value;
    Poco::Timestamp expiresAt;
    bool isExpired() const { return Poco::Timestamp() > expiresAt; }
};

class CacheService {
public:
    explicit CacheService(const std::string& prefix = "med:") : prefix_(prefix) {
        Poco::Logger::get("Cache").information("In-memory cache initialized (prefix: %s)", prefix_);
    }
    
    /// Возвращает значение или пустую строку, если не найдено/истекло
    std::string get(const std::string& key) {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = cache_.find(prefix_ + key);
        
        if (it != cache_.end() && !it->second.isExpired()) {
            Poco::Logger::get("Cache").debug("CACHE_HIT: %s", (prefix_ + key).c_str());
            return it->second.value;
        }
        if (it != cache_.end()) cache_.erase(it);
        Poco::Logger::get("Cache").debug("CACHE_MISS: %s", (prefix_ + key).c_str());
        return "";  // пустая строка = промах
    }
    
    void set(const std::string& key, const std::string& value, int ttlSeconds) {
        std::lock_guard<std::mutex> lock(mutex_);
        CacheEntry e{value, Poco::Timestamp() + (ttlSeconds * 1000000)};
        cache_[prefix_ + key] = e;
    }
    
    void del(const std::string& key) {
        std::lock_guard<std::mutex> lock(mutex_);
        cache_.erase(prefix_ + key);
    }
    
    void invalidatePattern(const std::string& pattern) {
        std::lock_guard<std::mutex> lock(mutex_);
        std::string full = prefix_ + pattern;
        for (auto it = cache_.begin(); it != cache_.end(); ) {
            if (it->first.find(full) == 0) it = cache_.erase(it);
            else ++it;
        }
    }

private:
    std::unordered_map<std::string, CacheEntry> cache_;
    mutable std::mutex mutex_;
    std::string prefix_;
};

} // namespace cache