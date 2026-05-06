#pragma once

#include <string>
#include <unordered_map>
#include <mutex>
#include <algorithm>
#include <Poco/Timestamp.h>
#include <Poco/Logger.h>

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
        : defaultLimit_(defaultLimit)
        , windowSeconds_(windowSeconds)
        , prefix_(prefix)
    {
        auto& logger = Poco::Logger::get("RateLimiter");
        logger.information(
            "Fixed Window limiter: %d requests per %d seconds",
            defaultLimit_,
            windowSeconds_
        );
    }

    RateLimitResult check(const std::string& key, int limitOverride = -1)
    {
        int limit = defaultLimit_;
        if (limitOverride > 0) {
            limit = limitOverride;
        }
        std::string fullKey = prefix_ + key;
        Poco::Timestamp now;
        std::lock_guard<std::mutex> lock(mutex_);
        RateEntry& entry = limits_[fullKey];

        int64_t elapsed_us = now - entry.windowStart;
        int elapsed_seconds = static_cast<int>(elapsed_us / 1000000);

        if (elapsed_seconds >= windowSeconds_) {
            entry.count = 0;
            entry.windowStart = now;
        }

        entry.count++;
        int remaining = std::max(0, limit - entry.count);
        int seconds_left = windowSeconds_ - elapsed_seconds;
        int reset = std::max(1, seconds_left);

        return {
            entry.count <= limit,
            limit,
            remaining,
            reset
        };
    }

    void reset(const std::string& key)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        limits_.erase(prefix_ + key);
    }

private:
    std::unordered_map<std::string, RateEntry> limits_;
    mutable std::mutex mutex_;
    int defaultLimit_;
    int windowSeconds_;
    std::string prefix_;
};

} // namespace cache