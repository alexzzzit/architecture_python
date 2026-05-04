#pragma once
#include "Poco/Net/HTTPRequestHandler.h"
#include "Poco/Net/HTTPServerRequest.h"
#include "Poco/Net/HTTPServerResponse.h"

namespace handlers {

// Класс UserHandler обрабатывает HTTP запросы к эндпоинтам пользователей
// Обрабатывает маршруты: /api/v1/users, /api/v1/users/login/{login}, /api/v1/users/search
// Соответствует REST API для управления пользователями
class UserHandler : public Poco::Net::HTTPRequestHandler {
public:
    // Главный метод обработки HTTP запроса
    // request: входящий HTTP запрос (метод, URI, заголовки, тело)
    // response: исходящий HTTP ответ (статус, заголовки, тело)
    void handleRequest(Poco::Net::HTTPServerRequest& request,
                       Poco::Net::HTTPServerResponse& response) override;
};

}
