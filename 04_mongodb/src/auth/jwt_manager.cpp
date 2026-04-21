#include "auth/jwt_manager.h"
#include <mutex>
#include <random>
#include <sstream>
#include <iomanip>

namespace auth {

TokenManager& TokenManager::instance() {
    static TokenManager instance;
    return instance;
}

std::string TokenManager::generateToken(const std::string& userId, const std::string& role) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    // Простая генерация токена (для продакшена использовать реальную JWT библиотеку)
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 15);
    
    std::stringstream ss;
    ss << std::hex;
    for (int i = 0; i < 32; ++i) {
        ss << dis(gen);
    }
    
    std::string token = ss.str();
    tokens_[token] = {userId, role};
    return token;
}

std::optional<std::string> TokenManager::validateToken(const std::string& token) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = tokens_.find(token);
    if (it != tokens_.end()) {
        return it->second.first; // userId
    }
    return std::nullopt;
}

void TokenManager::revokeToken(const std::string& token) {
    std::lock_guard<std::mutex> lock(mutex_);
    tokens_.erase(token);
}

void TokenManager::setSecret(const std::string& secret) {
    std::lock_guard<std::mutex> lock(mutex_);
    secret_ = secret;
}

} // namespace auth