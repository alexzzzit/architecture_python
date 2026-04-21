#pragma once

#include <string>
#include <optional>
#include <unordered_map>
#include <mutex>

namespace auth {

class TokenManager {
public:
    static TokenManager& instance();

    std::string generateToken(const std::string& userId, const std::string& role);
    std::optional<std::string> validateToken(const std::string& token);
    void revokeToken(const std::string& token);
    void setSecret(const std::string& secret);

private:
    TokenManager() = default;
    ~TokenManager() = default;

    TokenManager(const TokenManager&) = delete;
    TokenManager& operator=(const TokenManager&) = delete;

    std::string secret_ = "default-secret-change-me";
    std::unordered_map<std::string, std::pair<std::string, std::string>> tokens_;
    mutable std::mutex mutex_;
};

} // namespace auth