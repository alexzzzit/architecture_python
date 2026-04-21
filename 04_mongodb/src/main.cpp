#include <iostream>
#include <fstream>
#include <sstream>
#include <random>
#include <unordered_map>
#include <mutex>

#include <Poco/Net/HTTPServer.h>
#include <Poco/Net/ServerSocket.h>
#include <Poco/Net/HTTPServerParams.h>
#include <Poco/Net/HTTPRequestHandler.h>
#include <Poco/Net/HTTPRequestHandlerFactory.h>
#include <Poco/Net/HTTPServerRequest.h>
#include <Poco/Net/HTTPServerResponse.h>
#include <Poco/Net/HTMLForm.h>
#include <Poco/Util/ServerApplication.h>
#include <Poco/Logger.h>
#include <Poco/Message.h>
#include <Poco/Environment.h>
#include <Poco/NumberParser.h>
#include <Poco/Exception.h>
#include <Poco/File.h>
#include <Poco/FileStream.h>
#include <Poco/StreamCopier.h>
#include <Poco/JSON/Object.h>
#include <Poco/JSON/Parser.h>
#include <Poco/JSON/Stringifier.h>

using namespace Poco;
using namespace Poco::Net;
using namespace Poco::Util;
using namespace Poco::JSON;

// =============================================================================
// Вспомогательные функции
// =============================================================================

// Простая генерация токена (для демо, не настоящий JWT)
std::string generateToken(const std::string& userId) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 15);
    std::stringstream ss;
    ss << std::hex;
    for (int i = 0; i < 32; ++i) ss << dis(gen);
    return userId + "." + ss.str();
}

// Проверка токена (упрощённая)
bool validateToken(const std::string& token) {
    return !token.empty() && token.find('.') != std::string::npos;
}

// Извлечение userId из токена
std::string extractUserId(const std::string& token) {
    size_t pos = token.find('.');
    if (pos != std::string::npos) return token.substr(0, pos);
    return "";
}

// Чтение файла в строку
std::string readFile(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) return "";
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

// Отправка JSON-ответа
void sendJson(HTTPServerResponse& response, const std::string& json, HTTPResponse::HTTPStatus status = HTTPResponse::HTTP_OK) {
    response.setContentType("application/json");
    response.setStatusAndReason(status);
    response.send() << json;
}

// Отправка ошибки
void sendError(HTTPServerResponse& response, const std::string& message, HTTPResponse::HTTPStatus status = HTTPResponse::HTTP_BAD_REQUEST) {
    Object obj;
    obj.set("error", message);
    obj.set("status", status);
    std::string json;
    StringWriter::stringify(obj, json);
    sendJson(response, json, status);
}

// Извлечение токена из заголовка Authorization
std::string extractTokenFromHeader(const std::string& authHeader) {
    if (authHeader.size() > 7 && authHeader.substr(0, 7) == "Bearer ") {
        return authHeader.substr(7);
    }
    return "";
}

// =============================================================================
// Обработчики запросов
// =============================================================================

class HealthHandler : public HTTPRequestHandler {
public:
    void handleRequest(HTTPServerRequest&, HTTPServerResponse& response) override {
        sendJson(response, R"({"status":"ok","service":"medical-records-mongodb"})");
    }
};

class SwaggerHandler : public HTTPRequestHandler {
public:
    void handleRequest(HTTPServerRequest&, HTTPServerResponse& response) override {
        std::string content = readFile("/app/openapi.yaml");
        if (content.empty()) {
            sendError(response, "OpenAPI spec not found", HTTPResponse::HTTP_NOT_FOUND);
            return;
        }
        response.setContentType("application/yaml");
        response.setStatusAndReason(HTTPResponse::HTTP_OK);
        response.send() << content;
    }
};

class AuthHandler : public HTTPRequestHandler {
public:
    void handleRequest(HTTPServerRequest& request, HTTPServerResponse& response) override {
        if (request.getMethod() != HTTPRequest::HTTP_POST) {
            sendError(response, "Method not allowed", HTTPResponse::HTTP_METHOD_NOT_ALLOWED);
            return;
        }

        try {
            HTMLForm form(request, request.stream());
            std::string login = form.get("login", "");
            std::string password = form.get("password", "");

            // Упрощённая проверка (в продакшене — проверка в БД)
            if (login == "doctor_smirnov" && password == "password123") {
                Object obj;
                obj.set("token", generateToken(login));
                obj.set("user", login);
                obj.set("role", "doctor");
                std::string json;
                StringWriter::stringify(obj, json);
                sendJson(response, json);
            } else {
                sendError(response, "Invalid credentials", HTTPResponse::HTTP_UNAUTHORIZED);
            }
        } catch (...) {
            sendError(response, "Invalid request body");
        }
    }
};

class UserLoginHandler : public HTTPRequestHandler {
public:
    void handleRequest(HTTPServerRequest& request, HTTPServerResponse& response) override {
        // Проверка авторизации
        std::string token = extractTokenFromHeader(request.get("Authorization", ""));
        if (!validateToken(token)) {
            sendError(response, "Unauthorized", HTTPResponse::HTTP_UNAUTHORIZED);
            return;
        }

        // Извлечение логина из пути: /api/v1/users/login/{login}
        std::string path = request.getURI();
        size_t pos = path.rfind('/');
        if (pos == std::string::npos) {
            sendError(response, "Invalid path");
            return;
        }
        std::string login = path.substr(pos + 1);

        // Mock-ответ (в продакшене — запрос в БД)
        Object obj;
        obj.set("id", 1);
        obj.set("login", login);
        obj.set("email", login + "@hospital.ru");
        obj.set("role", "doctor");
        obj.set("is_active", true);
        std::string json;
        StringWriter::stringify(obj, json);
        sendJson(response, json);
    }
};

class UserSearchHandler : public HTTPRequestHandler {
public:
    void handleRequest(HTTPServerRequest& request, HTTPServerResponse& response) override {
        // Проверка авторизации
        std::string token = extractTokenFromHeader(request.get("Authorization", ""));
        if (!validateToken(token)) {
            sendError(response, "Unauthorized", HTTPResponse::HTTP_UNAUTHORIZED);
            return;
        }

        // Получение параметра mask из query string
        std::string mask = request.get("mask", "");
        
        // Mock-ответ с поиском по маске
        Array users;
        if (!mask.empty()) {
            Object u1, u2;
            u1.set("id", 1); u1.set("login", "doctor_smirnov"); u1.set("first_name", "Иван"); u1.set("last_name", "Смирнов");
            u2.set("id", 2); u2.set("login", "patient_ivanov"); u2.set("first_name", "Иван"); u2.set("last_name", "Иванов");
            users.add(u1); users.add(u2);
        }
        
        Object result;
        result.set("users", users);
        result.set("total", users.size());
        std::string json;
        StringWriter::stringify(result, json);
        sendJson(response, json);
    }
};

class PatientRecordsHandler : public HTTPRequestHandler {
public:
    void handleRequest(HTTPServerRequest& request, HTTPServerResponse& response) override {
        // Проверка авторизации
        std::string token = extractTokenFromHeader(request.get("Authorization", ""));
        if (!validateToken(token)) {
            sendError(response, "Unauthorized", HTTPResponse::HTTP_UNAUTHORIZED);
            return;
        }

        // Извлечение patient_id из пути
        std::string path = request.getURI();
        size_t start = path.find("/patients/") + 10;
        size_t end = path.find("/records", start);
        if (start == std::string::npos || end == std::string::npos) {
            sendError(response, "Invalid path");
            return;
        }
        std::string patientId = path.substr(start, end - start);

        // Mock-ответ с записями пациента
        Array records;
        Object r1, r2;
        r1.set("id", 1); r1.set("code", "REC-2024-001"); r1.set("diagnosis", "Острый бронхит"); r1.set("status", "confirmed");
        r2.set("id", 2); r2.set("code", "REC-2024-002"); r2.set("diagnosis", "Профилактический осмотр"); r2.set("status", "confirmed");
        records.add(r1); records.add(r2);

        Object result;
        result.set("patient_id", patientId);
        result.set("records", records);
        result.set("total", records.size());
        std::string json;
        StringWriter::stringify(result, json);
        sendJson(response, json);
    }
};

class RecordByCodeHandler : public HTTPRequestHandler {
public:
    void handleRequest(HTTPServerRequest& request, HTTPServerResponse& response) override {
        // Проверка авторизации
        std::string token = extractTokenFromHeader(request.get("Authorization", ""));
        if (!validateToken(token)) {
            sendError(response, "Unauthorized", HTTPResponse::HTTP_UNAUTHORIZED);
            return;
        }

        // Извлечение кода из пути: /api/v1/records/{code}
        std::string path = request.getURI();
        size_t pos = path.rfind('/');
        if (pos == std::string::npos) {
            sendError(response, "Invalid path");
            return;
        }
        std::string code = path.substr(pos + 1);

        // Mock-ответ
        Object record;
        record.set("id", 1);
        record.set("code", code);
        record.set("patient_id", 1);
        record.set("doctor_id", 1);
        record.set("diagnosis", "Гипертония 1 степени");
        record.set("status", "confirmed");
        record.set("visit_date", "2024-04-10T09:15:00Z");
        
        Object treatmentPlan;
        Array medications;
        Object med;
        med.set("name", "Лизиноприл");
        med.set("dosage", "10мг");
        med.set("frequency", "1 раз в день");
        medications.add(med);
        treatmentPlan.set("medications", medications);
        treatmentPlan.set("recommendations", "Ограничение соли, контроль веса");
        record.set("treatment_plan", treatmentPlan);

        std::string json;
        StringWriter::stringify(record, json);
        sendJson(response, json);
    }
};

class NotFoundHandler : public HTTPRequestHandler {
public:
    void handleRequest(HTTPServerRequest&, HTTPServerResponse& response) override {
        sendError(response, "Not found", HTTPResponse::HTTP_NOT_FOUND);
    }
};

// =============================================================================
// Фабрика маршрутов
// =============================================================================

class RouterFactory : public HTTPRequestHandlerFactory {
public:
    HTTPRequestHandler* createRequestHandler(const HTTPServerRequest& request) override {
        const std::string& path = request.getURI();
        const std::string& method = request.getMethod();

        // Health check
        if (path == "/health" || path == "/api/health") {
            return new HealthHandler();
        }

        // Swagger/OpenAPI spec
        if (path == "/swagger.yaml" || path == "/api/swagger.yaml") {
            return new SwaggerHandler();
        }

        // Auth endpoints
        if (path == "/api/v1/auth/login" && method == HTTPRequest::HTTP_POST) {
            return new AuthHandler();
        }

        // User endpoints
        if (path.find("/api/v1/users/login/") == 0 && method == HTTPRequest::HTTP_GET) {
            return new UserLoginHandler();
        }
        if (path.find("/api/v1/users/search") == 0 && method == HTTPRequest::HTTP_GET) {
            return new UserSearchHandler();
        }

        // Patient endpoints
        if (path.find("/api/v1/patients/") != std::string::npos && 
            path.find("/records") != std::string::npos && 
            method == HTTPRequest::HTTP_GET) {
            return new PatientRecordsHandler();
        }

        // Record endpoints
        if (path.find("/api/v1/records/") == 0 && 
            path.find("/records/") != std::string::npos && 
            method == HTTPRequest::HTTP_GET) {
            return new RecordByCodeHandler();
        }

        // Not found
        return new NotFoundHandler();
    }
};

// =============================================================================
// Настройка и запуск приложения
// =============================================================================

void configureLogging() {
    std::string level = Environment::get("LOG_LEVEL", "information");
    Poco::Message::Priority prio = Poco::Message::PRIO_INFORMATION;

    if (level == "trace") prio = Poco::Message::PRIO_TRACE;
    else if (level == "debug") prio = Poco::Message::PRIO_DEBUG;
    else if (level == "information" || level == "info") prio = Poco::Message::PRIO_INFORMATION;
    else if (level == "warning" || level == "warn") prio = Poco::Message::PRIO_WARNING;
    else if (level == "error") prio = Poco::Message::PRIO_ERROR;
    else if (level == "critical") prio = Poco::Message::PRIO_CRITICAL;
    else if (level == "fatal") prio = Poco::Message::PRIO_FATAL;
    else if (level == "none") prio = static_cast<Poco::Message::Priority>(Poco::Message::PRIO_FATAL + 1);

    Logger::root().setLevel(prio);
}

class ServerApp : public ServerApplication {
protected:
    int main(const std::vector<std::string>&) override {
        configureLogging();
        auto& logger = Logger::get("Server");

        unsigned short port = 8080;
        if (Environment::has("PORT")) {
            try {
                port = static_cast<unsigned short>(NumberParser::parse(Environment::get("PORT")));
            } catch (const Exception& e) {
                logger.warning("Invalid PORT, using default 8080: %s", e.displayText());
            }
        }

        logger.information("Starting medical server on port %hu", port);

        ServerSocket svs(port);
        HTTPServer srv(new RouterFactory(), svs, new HTTPServerParams);
        srv.start();

        logger.information("Server started. Endpoints: /health, /swagger.yaml, /api/v1/*");

        waitForTerminationRequest();
        srv.stop();

        logger.information("Server stopped");
        return Application::EXIT_OK;
    }
};

POCO_SERVER_MAIN(ServerApp)