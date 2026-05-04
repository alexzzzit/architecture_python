#pragma once
#include "Poco/Net/HTTPRequestHandler.h"
#include "Poco/Net/HTTPServerRequest.h"
#include "Poco/Net/HTTPServerResponse.h"

namespace handlers {

// Класс AuthHandler обрабатывает HTTP запросы к эндпоинтам аутентификации
// Обрабатывает маршрут: /api/v1/auth/login
// Соответствует REST API для аутентификации пользователей
class AuthHandler : public Poco::Net::HTTPRequestHandler {
public:
    // Главный метод обработки HTTP запроса
    // request: входящий HTTP запрос (метод, URI, заголовки, тело)
    // response: исходящий HTTP ответ (статус, заголовки, тело)
    void handleRequest(Poco::Net::HTTPServerRequest& request,
                       Poco::Net::HTTPServerResponse& response) override;
};

// Класс HealthHandler обрабатывает запросы проверки здоровья сервиса
// Обрабатывает маршрут: /health
// Используется для health checks в Docker и мониторинга
class HealthHandler : public Poco::Net::HTTPRequestHandler {
public:
    // Главный метод обработки HTTP запроса
    // request: входящий HTTP запрос
    // response: исходящий HTTP ответ со статусом сервиса
    void handleRequest(Poco::Net::HTTPServerRequest& request,
                       Poco::Net::HTTPServerResponse& response) override;
};

// Класс SwaggerHandler отдаёт OpenAPI спецификацию
// Обрабатывает маршрут: /swagger.yaml
// Используется для документирования API и Swagger UI
class SwaggerHandler : public Poco::Net::HTTPRequestHandler {
public:
    // Главный метод обработки HTTP запроса
    // request: входящий HTTP запрос
    // response: исходящий HTTP ответ с YAML файлом спецификации
    void handleRequest(Poco::Net::HTTPServerRequest& request,
                       Poco::Net::HTTPServerResponse& response) override;
};

// Класс NotFoundHandler обрабатывает запросы к несуществующим эндпоинтам
// Возвращает HTTP 404 для всех неизвестных маршрутов
class NotFoundHandler : public Poco::Net::HTTPRequestHandler {
public:
    // Главный метод обработки HTTP запроса
    // request: входящий HTTP запрос
    // response: исходящий HTTP ответ с ошибкой 404
    void handleRequest(Poco::Net::HTTPServerRequest& request,
                       Poco::Net::HTTPServerResponse& response) override;
};

}
