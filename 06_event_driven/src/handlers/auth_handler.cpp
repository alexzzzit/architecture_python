#include "handlers/auth_handler.h"
#include "auth/jwt_manager.h"
#include "services/user_service.h"
#include "utils/json_response.h"
#include "Poco/JSON/Parser.h"
#include "Poco/StreamCopier.h"
#include "Poco/Logger.h"
#include <sstream>
#include <fstream>

using namespace Poco::Net;

namespace handlers {

void AuthHandler::handleRequest(HTTPServerRequest& request, HTTPServerResponse& response) {
    if (request.getURI() == "/api/v1/auth/login" && request.getMethod() == "POST") {
        std::stringstream ss;
        Poco::StreamCopier::copyStream(request.stream(), ss);
        std::string body = ss.str();

        try {
            Poco::JSON::Parser parser;
            auto result = parser.parse(body);
            auto root = result.extract<Poco::JSON::Object::Ptr>();

            std::string login = root->getValue<std::string>("login");
            std::string password = root->getValue<std::string>("password");

            auto user = services::UserService::getInstance().findByLogin(login);
            if (!user || !services::UserService::getInstance().validatePassword(user.value(), password)) {
                utils::JsonResponse::sendError(response, 401, "Invalid credentials");
                return;
            }

            std::string token = auth::TokenManager::getInstance().generateToken(
                user.value().id, user.value().role);

            Poco::JSON::Object::Ptr resp = new Poco::JSON::Object();
            resp->set("token", token);
            resp->set("userId", user.value().id);
            resp->set("role", user.value().role);

            utils::JsonResponse::sendJSON(response, 200, resp);
        } catch (const Poco::Exception& ex) {
            utils::JsonResponse::sendError(response, 400, "Invalid request body");
        }
        return;
    }

    utils::JsonResponse::sendError(response, 404, "Endpoint not found");
}

void HealthHandler::handleRequest(HTTPServerRequest& request, HTTPServerResponse& response) {
    Poco::JSON::Object::Ptr resp = new Poco::JSON::Object();
    resp->set("status", "healthy");
    resp->set("service", "medical-records-api");
    resp->set("version", "1.0.0");
    resp->set("timestamp", Poco::Timestamp().epochTime());
    utils::JsonResponse::sendJSON(response, 200, resp);
}

void SwaggerHandler::handleRequest(HTTPServerRequest& request, HTTPServerResponse& response) {
    response.setContentType("application/x-yaml");
    std::ifstream file("docs/openapi.yaml");
    if (file.is_open()) {
        Poco::StreamCopier::copyStream(file, response.send());
    } else {
        utils::JsonResponse::sendError(response, 404, "OpenAPI spec not found");
    }
}

void NotFoundHandler::handleRequest(HTTPServerRequest& request, HTTPServerResponse& response) {
    utils::JsonResponse::sendError(response, 404, "Not Found");
}

}