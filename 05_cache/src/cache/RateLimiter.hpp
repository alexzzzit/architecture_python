// src/cache/RateLimiter.hpp — без изменений, совместим с POCO 1.9+
#pragma once
#include <Poco/Timestamp.h>
#include <Poco/Logger.h>
#include <string>
#include <unordered_map>
#include <mutex>
#include <algorithm>

namespace cache {

struct RateLimitResult {
    bool allowed;
    int limit;
    int remaining;
    int resetSeconds;
};

struct RateEntry {
    int count = 0;
    Poco::Timestamp windowStart;
};

class RateLimiter {
public:
    RateLimiter(int defaultLimit = 60, int windowSeconds = 60, const std::string& prefix = "rl:")
        : defaultLimit_(defaultLimit), windowSeconds_(windowSeconds), prefix_(prefix) {
        Poco::Logger::get("RateLimiter").information("Fixed Window limiter: %d req/%ds", defaultLimit_, windowSeconds_);
    }

    RateLimitResult check(const std::string& key, int limitOverride = -1) {
        int limit = limitOverride > 0 ? limitOverride : defaultLimit_;
        std::string fullKey = prefix_ + key;
        Poco::Timestamp now;
        
        std::lock_guard<std::mutex> lock(mutex_);
        auto& entry = limits_[fullKey];
        
        if ((now - entry.windowStart) / 1000000 >= windowSeconds_) {
            entry.count = 0;
            entry.windowStart = now;
        }
        
        entry.count++;
        int remaining = std::max(0, limit - entry.count);
        int elapsed = static_cast<int>((now - entry.windowStart) / 1000000);
        int reset = std::max(1, windowSeconds_ - elapsed);
        
        return {entry.count <= limit, limit, remaining, reset};
    }

    void reset(const std::string& key) {
        std::lock_guard<std::mutex> lock(mutex_);
        limits_.erase(prefix_ + key);
    }

private:
    std::unordered_map<std::string, RateEntry> limits_;
    mutable std::mutex mutex_;
    int defaultLimit_, windowSeconds_;
    std::string prefix_;
};

} // namespace cache