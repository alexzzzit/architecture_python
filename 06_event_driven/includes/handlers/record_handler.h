#pragma once
#include "Poco/Net/HTTPRequestHandler.h"
#include "Poco/Net/HTTPServerRequest.h"
#include "Poco/Net/HTTPServerResponse.h"

namespace handlers {

// Класс RecordHandler обрабатывает HTTP запросы к эндпоинтам медицинских записей
// Обрабатывает маршруты: /api/v1/records, /api/v1/records/{code}
// Соответствует REST API для управления медицинскими записями
class RecordHandler : public Poco::Net::HTTPRequestHandler {
public:
    // Главный метод обработки HTTP запроса
    // request: входящий HTTP запрос (метод, URI, заголовки, тело)
    // response: исходящий HTTP ответ (статус, заголовки, тело)
    void handleRequest(Poco::Net::HTTPServerRequest& request,
                       Poco::Net::HTTPServerResponse& response) override;
};

}
