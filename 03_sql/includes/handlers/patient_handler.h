#pragma once
#include "Poco/Net/HTTPRequestHandler.h"
#include "Poco/Net/HTTPServerRequest.h"
#include "Poco/Net/HTTPServerResponse.h"

namespace handlers {

// Класс PatientHandler обрабатывает HTTP запросы к эндпоинтам пациентов
// Обрабатывает маршруты: /api/v1/patients, /api/v1/patients/search, /api/v1/patients/{id}/records
// Соответствует REST API для управления пациентами и их записями
class PatientHandler : public Poco::Net::HTTPRequestHandler {
public:
    // Главный метод обработки HTTP запроса
    // request: входящий HTTP запрос (метод, URI, заголовки, тело)
    // response: исходящий HTTP ответ (статус, заголовки, тело)
    void handleRequest(Poco::Net::HTTPServerRequest& request,
                       Poco::Net::HTTPServerResponse& response) override;
};

}
