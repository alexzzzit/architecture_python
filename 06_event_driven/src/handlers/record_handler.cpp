#include "handlers/record_handler.h"
#include "auth/jwt_manager.h"
#include "services/record_service.h"
#include "utils/json_response.h"
#include "Poco/JSON/Parser.h"
#include "Poco/StreamCopier.h"
#include "Poco/Logger.h"
#include <sstream>
#include <regex>

using namespace Poco::Net;

namespace handlers {

void RecordHandler::handleRequest(HTTPServerRequest& request, HTTPServerResponse& response) {
    const std::string& uri = request.getURI();
    const std::string& method = request.getMethod();

    std::string authHeader = request.get("Authorization", "");
    std::string token = auth::TokenManager::getInstance().extractTokenFromHeader(authHeader);
    auto payload = auth::TokenManager::getInstance().validateToken(token);
    
    if (!payload) {
        utils::JsonResponse::sendError(response, 401, "Unauthorized");
        return;
    }

    std::regex codeRegex(R"(/api/v1/records/([^/]+))");
    std::smatch match;
    if (std::regex_match(uri, match, codeRegex) && method == "GET") {
        std::string code = match[1];
        auto record = services::RecordService::getInstance().findByCode(code);
        if (record) {
            Poco::JSON::Object::Ptr resp = new Poco::JSON::Object();
            resp->set("code", record.value().code);
            resp->set("patientId", record.value().patientId);
            resp->set("doctorId", record.value().doctorId);
            resp->set("diagnosis", record.value().diagnosis);
            resp->set("treatment", record.value().treatment);
            resp->set("notes", record.value().notes);
            resp->set("createdAt", record.value().createdAt);
            utils::JsonResponse::sendJSON(response, 200, resp);
        } else {
            utils::JsonResponse::sendError(response, 404, "Record not found");
        }
        return;
    }

    if (uri == "/api/v1/records" && method == "POST") {
        std::stringstream ss;
        Poco::StreamCopier::copyStream(request.stream(), ss);
        std::string body = ss.str();

        try {
            Poco::JSON::Parser parser;
            auto result = parser.parse(body);
            auto root = result.extract<Poco::JSON::Object::Ptr>();

            models::RecordCreateRequest req;
            req.patientId = root->getValue<std::string>("patientId");
            req.doctorId = payload.value().first;
            req.diagnosis = root->getValue<std::string>("diagnosis");
            req.treatment = root->getValue<std::string>("treatment");
            
            if (root->has("notes")) {
                req.notes = root->getValue<std::string>("notes");
            } else {
                req.notes = "";
            }

            auto record = services::RecordService::getInstance().create(req);
            if (record) {
                Poco::JSON::Object::Ptr resp = new Poco::JSON::Object();
                resp->set("code", record.value().code);
                resp->set("patientId", record.value().patientId);
                resp->set("diagnosis", record.value().diagnosis);
                resp->set("createdAt", record.value().createdAt);
                
                utils::JsonResponse::sendJSON(response, 201, resp);
            } else {
                utils::JsonResponse::sendError(response, 400, "Record creation failed");
            }
        } catch (const Poco::Exception& ex) {
            utils::JsonResponse::sendError(response, 400, "Invalid request body");
        }
        return;
    }

    utils::JsonResponse::sendError(response, 404, "Endpoint not found");
}

}