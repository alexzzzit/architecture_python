#include "handlers/user_handler.h"
#include "handlers/auth_handler.h"
#include "auth/jwt_manager.h"
#include "services/user_service.h"
#include "utils/json_response.h"
#include "Poco/JSON/Parser.h"
#include "Poco/StreamCopier.h"
#include "Poco/Logger.h"
#include <sstream>
#include <regex>

using namespace Poco::Net;

namespace handlers {

void UserHandler::handleRequest(HTTPServerRequest& request, HTTPServerResponse& response) {
    const std::string& uri = request.getURI();
    const std::string& method = request.getMethod();

    if (uri == "/api/v1/users" && method == "POST") {
        std::stringstream ss;
        Poco::StreamCopier::copyStream(request.stream(), ss);
        std::string body = ss.str();

        try {
            Poco::JSON::Parser parser;
            auto result = parser.parse(body);
            auto root = result.extract<Poco::JSON::Object::Ptr>();

            models::UserCreateRequest req;
            req.login = root->getValue<std::string>("login");
            req.firstName = root->getValue<std::string>("firstName");
            req.lastName = root->getValue<std::string>("lastName");
            req.email = root->getValue<std::string>("email");
            req.password = root->getValue<std::string>("password");
            
            if (root->has("role")) {
                req.role = root->getValue<std::string>("role");
            } else {
                req.role = "patient";
            }

            auto user = services::UserService::getInstance().create(req);
            if (user) {
                Poco::JSON::Object::Ptr resp = new Poco::JSON::Object();
                resp->set("id", user.value().id);
                resp->set("login", user.value().login);
                resp->set("firstName", user.value().firstName);
                resp->set("lastName", user.value().lastName);
                resp->set("email", user.value().email);
                resp->set("role", user.value().role);
                resp->set("createdAt", user.value().createdAt);
                
                utils::JsonResponse::sendJSON(response, 201, resp);
            } else {
                utils::JsonResponse::sendError(response, 400, "User creation failed (login exists)");
            }
        } catch (const Poco::Exception& ex) {
            utils::JsonResponse::sendError(response, 400, "Invalid request body");
        }
        return;
    }

    std::string authHeader = request.get("Authorization", "");
    std::string token = auth::TokenManager::getInstance().extractTokenFromHeader(authHeader);
    auto payload = auth::TokenManager::getInstance().validateToken(token);
    
    if (!payload) {
        utils::JsonResponse::sendError(response, 401, "Unauthorized");
        return;
    }

    std::regex loginRegex(R"(/api/v1/users/login/([^/]+))");
    std::smatch match;
    if (std::regex_match(uri, match, loginRegex) && method == "GET") {
        std::string login = match[1];
        auto user = services::UserService::getInstance().findByLogin(login);
        if (user) {
            Poco::JSON::Object::Ptr resp = new Poco::JSON::Object();
            resp->set("id", user.value().id);
            resp->set("login", user.value().login);
            resp->set("firstName", user.value().firstName);
            resp->set("lastName", user.value().lastName);
            resp->set("email", user.value().email);
            resp->set("role", user.value().role);
            utils::JsonResponse::sendJSON(response, 200, resp);
        } else {
            utils::JsonResponse::sendError(response, 404, "User not found");
        }
        return;
    }

    if (uri.find("/api/v1/users/search") == 0 && method == "GET") {
        std::string firstName = request.get("firstName", "");
        std::string lastName = request.get("lastName", "");
        
        auto users = services::UserService::getInstance().searchByName(firstName, lastName);
        Poco::JSON::Array::Ptr arr = new Poco::JSON::Array();
        for (const auto& u : users) {
            Poco::JSON::Object::Ptr obj = new Poco::JSON::Object();
            obj->set("id", u.id);
            obj->set("login", u.login);
            obj->set("firstName", u.firstName);
            obj->set("lastName", u.lastName);
            obj->set("email", u.email);
            obj->set("role", u.role);
            arr->add(obj);
        }
        utils::JsonResponse::sendJSON(response, 200, arr);
        return;
    }

    utils::JsonResponse::sendError(response, 404, "Endpoint not found");
}

}