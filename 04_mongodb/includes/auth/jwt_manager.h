#pragma once
#include "Poco/JSON/Object.h"
#include <string>
#include <optional>
#include <unordered_map>
#include <mutex>

namespace auth {

class TokenManager {
public:
    static TokenManager& getInstance();
    
    std::string generateToken(const std::string& userId, const std::string& role);
    std::optional<std::pair<std::string, std::string>> validateToken(const std::string& token);
    std::string extractTokenFromHeader(const std::string& authHeader);
    void removeToken(const std::string& token);

private:
    TokenManager();
    TokenManager(const TokenManager&) = delete;
    TokenManager& operator=(const TokenManager&) = delete;
    
    std::string generateRandomToken();
    
    mutable std::mutex _mutex;
    std::unordered_map<std::string, std::pair<std::string, std::string>> _tokens;
};

}