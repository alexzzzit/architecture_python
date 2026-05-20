#pragma once
#include "Poco/Net/HTTPServerResponse.h"
#include "Poco/JSON/Object.h"
#include "Poco/JSON/Array.h"

namespace utils {

// Класс JsonResponse предоставляет утилиты для отправки JSON ответов
// Централизует логику форматирования и отправки JSON в HTTP ответе
class JsonResponse {
public:
    // Отправляет JSON объект в HTTP ответе
    // response: HTTP ответ для записи
    // statusCode: HTTP статус код (200, 201, 400, etc.)
    // data: JSON объект с данными ответа
    static void sendJSON(Poco::Net::HTTPServerResponse& response, 
                         int statusCode, 
                         Poco::JSON::Object::Ptr data);
    
    // Отправляет JSON массив в HTTP ответе
    // response: HTTP ответ для записи
    // statusCode: HTTP статус код
    // data: JSON массив с данными ответа
    static void sendJSON(Poco::Net::HTTPServerResponse& response, 
                         int statusCode, 
                         Poco::JSON::Array::Ptr data);
    
    // Отправляет JSON ответ с ошибкой
    // response: HTTP ответ для записи
    // statusCode: HTTP статус код ошибки (400, 401, 404, 500)
    // message: текстовое описание ошибки
    static void sendError(Poco::Net::HTTPServerResponse& response, 
                          int statusCode, 
                          const std::string& message);
};

}
