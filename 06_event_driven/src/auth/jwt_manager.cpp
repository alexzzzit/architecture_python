#include "auth/jwt_manager.h"
#include "Poco/RandomStream.h"
#include "Poco/DigestEngine.h"
#include "Poco/MD5Engine.h"
#include <sstream>
#include <iomanip>

namespace auth {

TokenManager::TokenManager() {}

TokenManager& TokenManager::getInstance() {
    static TokenManager instance;
    return instance;
}

std::string TokenManager::generateRandomToken() {
    Poco::RandomInputStream random;
    std::ostringstream oss;
    for (int i = 0; i < 16; ++i) {
        int byte = random.get();
        oss << std::hex << std::setfill('0') << std::setw(2) << byte;
    }
    return oss.str();
}

std::string TokenManager::generateToken(const std::string& userId, const std::string& role) {
    std::lock_guard<std::mutex> lock(_mutex);
    std::string token = generateRandomToken();
    _tokens[token] = {userId, role};
    return token;
}

std::optional<std::pair<std::string, std::string>> TokenManager::validateToken(const std::string& token) {
    std::lock_guard<std::mutex> lock(_mutex);
    auto it = _tokens.find(token);
    if (it != _tokens.end()) {
        return it->second;
    }
    return std::nullopt;
}

std::string TokenManager::extractTokenFromHeader(const std::string& authHeader) {
    const std::string prefix = "Bearer ";
    if (authHeader.size() > prefix.size() && 
        authHeader.substr(0, prefix.size()) == prefix) {
        return authHeader.substr(prefix.size());
    }
    return "";
}

void TokenManager::removeToken(const std::string& token) {
    std::lock_guard<std::mutex> lock(_mutex);
    _tokens.erase(token);
}

}