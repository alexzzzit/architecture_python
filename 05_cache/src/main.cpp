// src/main.cpp
#include "Poco/Net/HTTPServer.h"
#include "Poco/Net/ServerSocket.h"
#include "Poco/Net/HTTPRequestHandlerFactory.h"
#include "Poco/Net/HTTPRequestHandler.h"
#include "Poco/Net/HTTPResponse.h"
#include "Poco/Util/ServerApplication.h"
#include "Poco/Logger.h"
#include "Poco/AutoPtr.h"
#include "Poco/Environment.h"
#include "Poco/NumberParser.h"

#include <iostream>

#include "handlers/user_handler.h"
#include "handlers/patient_handler.h"
#include "handlers/record_handler.h"
#include "handlers/auth_handler.h"
#include "cache/CacheService.hpp"
#include "cache/RateLimiter.hpp"

using namespace Poco::Net;
using namespace Poco::Util;

// ============================================================================
// ГЛОБАЛЬНЫЕ СЕРВИСЫ (единое объявление)
// ============================================================================
cache::CacheService* g_cache = nullptr;
cache::RateLimiter* g_rateLimiter = nullptr;
static int g_rateLimitDefault = 60;
static int g_rateLimitWindow = 60;

// ============================================================================
// ВСПОМОГАТЕЛЬНЫЕ ФУНКЦИИ
// ============================================================================
std::string getClientIP(HTTPServerRequest& request) {
    std::string forwarded = request.get("X-Forwarded-For", "");
    if (!forwarded.empty()) {
        size_t comma = forwarded.find(',');
        return (comma != std::string::npos) ? forwarded.substr(0, comma) : forwarded;
    }
    return request.clientAddress().host().toString();
}

void addRateLimitHeaders(HTTPServerResponse& response, const cache::RateLimitResult& rl) {
    response.set("X-RateLimit-Limit", std::to_string(rl.limit));
    response.set("X-RateLimit-Remaining", std::to_string(rl.remaining));
    response.set("X-RateLimit-Reset", std::to_string(rl.resetSeconds));
    if (!rl.allowed) {
        response.set("Retry-After", std::to_string(rl.resetSeconds));
    }
}

// ============================================================================
// FACTORY
// ============================================================================
class MedicalRecordsHandlerFactory : public HTTPRequestHandlerFactory {
public:
    HTTPRequestHandler* createRequestHandler(const HTTPServerRequest& request) override {
        const std::string& uri = request.getURI();
        const std::string& method = request.getMethod();
        
        Poco::Logger::get("MedicalRecords").information("Request: %s %s", method, uri);

        if (uri.find("/api/v1/auth") == 0) {
            return new handlers::AuthHandler();
        }
        if (uri.find("/api/v1/users") == 0) {
            return new handlers::UserHandler();
        }
        if (uri.find("/api/v1/patients") == 0) {
            return new handlers::PatientHandler();
        }
        if (uri.find("/api/v1/records") == 0) {
            return new handlers::RecordHandler();
        }
        if (uri == "/health" || uri == "/api/v1/health") {
            return new handlers::HealthHandler();
        }
        if (uri == "/swagger.yaml" || uri == "/openapi.yaml") {
            return new handlers::SwaggerHandler();
        }

        return new handlers::NotFoundHandler();
    }
};

// ============================================================================
// SERVER APP
// ============================================================================
class MedicalRecordsApp : public ServerApplication {
protected:
    void initialize(Application& self) override {
        loadConfiguration();
        ServerApplication::initialize(self);
    }

    int main(const std::vector<std::string>&) override {
        auto& logger = Poco::Logger::get("MedicalRecords");
        logger.information("========================================");
        logger.information("Medical Records API v1.0.0 (Lab 05)");
        logger.information("========================================");

        // === Чтение конфигурации ===
        std::string portStr = Poco::Environment::get("PORT", "8080");
        uint16_t port = static_cast<uint16_t>(std::stoi(portStr));
        
        g_rateLimitDefault = Poco::NumberParser::parse(
            Poco::Environment::get("RATE_LIMIT_DEFAULT", "60"));
        g_rateLimitWindow = Poco::NumberParser::parse(
            Poco::Environment::get("RATE_LIMIT_WINDOW", "60"));

        // === Инициализация IN-MEMORY сервисов (без Redis) ===
        g_cache = new cache::CacheService("med:");
        g_rateLimiter = new cache::RateLimiter(g_rateLimitDefault, g_rateLimitWindow, "rl:");
        
        logger.information("Cache & RateLimiter initialized (in-memory mode)");
        logger.information("Rate limit: %d requests per %d seconds", g_rateLimitDefault, g_rateLimitWindow);

        // === Запуск HTTP сервера ===
        ServerSocket socket(port);
        HTTPServer server(new MedicalRecordsHandlerFactory(), socket, new HTTPServerParams);
        server.start();
        
        logger.information("Server listening on port %d", port);
        waitForTerminationRequest();
        
        logger.information("Shutting down...");
        server.stopAll();
        
        delete g_cache;
        delete g_rateLimiter;
        
        return Application::EXIT_OK;
    }
};

POCO_SERVER_MAIN(MedicalRecordsApp)