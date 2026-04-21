#include <iostream>
#include <Poco/Net/HTTPServer.h>
#include <Poco/Net/ServerSocket.h>
#include <Poco/Net/HTTPServerParams.h>
#include <Poco/Util/ServerApplication.h>
#include <Poco/Logger.h>
#include <Poco/Message.h>
#include <Poco/Environment.h>
#include <Poco/NumberParser.h>
#include <Poco/Exception.h>
#include <Poco/Net/HTTPRequestHandler.h>
#include <Poco/Net/HTTPRequestHandlerFactory.h>
#include <Poco/Net/HTTPServerRequest.h>
#include <Poco/Net/HTTPServerResponse.h>

using namespace Poco;
using namespace Poco::Net;
using namespace Poco::Util;

// Простой обработчик для /health
class HealthHandler : public HTTPRequestHandler {
public:
    void handleRequest(HTTPServerRequest& request, HTTPServerResponse& response) override {
        response.setContentType("application/json");
        response.setStatusAndReason(HTTPResponse::HTTP_OK);
        response.send() << "{\"status\":\"ok\",\"service\":\"medical-records-mongodb\"}";
    }
};

// Простой обработчик для /api/v1/users
class UsersHandler : public HTTPRequestHandler {
public:
    void handleRequest(HTTPServerRequest& request, HTTPServerResponse& response) override {
        response.setContentType("application/json");
        response.setStatusAndReason(HTTPResponse::HTTP_OK);
        response.send() << "{\"users\":[],\"total\":0}";
    }
};

// Фабрика маршрутов
class SimpleRouterFactory : public HTTPRequestHandlerFactory {
public:
    HTTPRequestHandler* createRequestHandler(const HTTPServerRequest& request) override {
        const std::string& path = request.getURI();
        const std::string& method = request.getMethod();

        if (path == "/health" || path == "/api/health") {
            return new HealthHandler();
        }
        if (path == "/api/v1/users" && method == HTTPRequest::HTTP_GET) {
            return new UsersHandler();
        }
        // Можно добавить другие хендлеры по мере готовности
        return nullptr; // вернёт 404
    }
};

// Настройка логирования
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

// Основное приложение
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

        // Запуск HTTP-сервера
        ServerSocket svs(port);
        HTTPServer srv(new SimpleRouterFactory(), svs, new HTTPServerParams);
        srv.start();

        logger.information("Server started. Endpoints: /health, /api/v1/users");

        waitForTerminationRequest();
        srv.stop();

        logger.information("Server stopped");
        return Application::EXIT_OK;
    }
};

POCO_SERVER_MAIN(ServerApp)