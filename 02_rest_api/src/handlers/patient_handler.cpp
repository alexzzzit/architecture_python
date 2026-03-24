#include "handlers/patient_handler.h"
#include "auth/jwt_manager.h"
#include "services/patient_service.h"
#include "utils/json_response.h"
#include "Poco/JSON/Parser.h"
#include "Poco/StreamCopier.h"
#include "Poco/Logger.h"
#include <sstream>
#include <regex>

using namespace Poco::Net;

namespace handlers {

void PatientHandler::handleRequest(HTTPServerRequest& request, HTTPServerResponse& response) {
    const std::string& uri = request.getURI();
    const std::string& method = request.getMethod();

    std::string authHeader = request.get("Authorization", "");
    std::string token = auth::TokenManager::getInstance().extractTokenFromHeader(authHeader);
    auto payload = auth::TokenManager::getInstance().validateToken(token);
    
    if (!payload) {
        utils::JsonResponse::sendError(response, 401, "Unauthorized");
        return;
    }

    if (uri == "/api/v1/patients" && method == "POST") {
        std::stringstream ss;
        Poco::StreamCopier::copyStream(request.stream(), ss);
        std::string body = ss.str();

        try {
            Poco::JSON::Parser parser;
            auto result = parser.parse(body);
            auto root = result.extract<Poco::JSON::Object::Ptr>();

            models::PatientCreateRequest req;
            req.userId = root->getValue<std::string>("userId");
            req.firstName = root->getValue<std::string>("firstName");
            req.lastName = root->getValue<std::string>("lastName");
            
            if (root->has("middleName")) {
                req.middleName = root->getValue<std::string>("middleName");
            } else {
                req.middleName = "";
            }
            
            req.birthDate = root->getValue<std::string>("birthDate");
            req.phone = root->getValue<std::string>("phone");
            req.insuranceNumber = root->getValue<std::string>("insuranceNumber");

            auto patient = services::PatientService::getInstance().create(req);
            if (patient) {
                Poco::JSON::Object::Ptr resp = new Poco::JSON::Object();
                resp->set("id", patient.value().id);
                resp->set("userId", patient.value().userId);
                resp->set("firstName", patient.value().firstName);
                resp->set("lastName", patient.value().lastName);
                resp->set("middleName", patient.value().middleName);
                resp->set("birthDate", patient.value().birthDate);
                resp->set("insuranceNumber", patient.value().insuranceNumber);
                resp->set("createdAt", patient.value().createdAt);
                
                utils::JsonResponse::sendJSON(response, 201, resp);
            } else {
                utils::JsonResponse::sendError(response, 400, "Patient creation failed");
            }
        } catch (const Poco::Exception& ex) {
            utils::JsonResponse::sendError(response, 400, "Invalid request body");
        }
        return;
    }

    if (uri.find("/api/v1/patients/search") == 0 && method == "GET") {
        std::string firstName = request.get("firstName", "");
        std::string lastName = request.get("lastName", "");
        std::string middleName = request.get("middleName", "");
        
        auto patients = services::PatientService::getInstance().searchByFullName(
            firstName, lastName, middleName);
        Poco::JSON::Array::Ptr arr = new Poco::JSON::Array();
        for (const auto& p : patients) {
            Poco::JSON::Object::Ptr obj = new Poco::JSON::Object();
            obj->set("id", p.id);
            obj->set("firstName", p.firstName);
            obj->set("lastName", p.lastName);
            obj->set("middleName", p.middleName);
            obj->set("birthDate", p.birthDate);
            obj->set("insuranceNumber", p.insuranceNumber);
            arr->add(obj);
        }
        utils::JsonResponse::sendJSON(response, 200, arr);
        return;
    }

    std::regex recordsRegex(R"(/api/v1/patients/([^/]+)/records)");
    std::smatch match;
    if (std::regex_match(uri, match, recordsRegex)) {
        std::string patientId = match[1];
        
        if (method == "GET") {
            auto records = services::PatientService::getInstance().getPatientRecords(patientId);
            Poco::JSON::Array::Ptr arr = new Poco::JSON::Array();
            for (const auto& r : records) {
                Poco::JSON::Object::Ptr obj = new Poco::JSON::Object();
                obj->set("code", r.code);
                obj->set("diagnosis", r.diagnosis);
                obj->set("treatment", r.treatment);
                obj->set("notes", r.notes);
                obj->set("createdAt", r.createdAt);
                arr->add(obj);
            }
            utils::JsonResponse::sendJSON(response, 200, arr);
            return;
        }
        
        if (method == "POST") {
            std::stringstream ss;
            Poco::StreamCopier::copyStream(request.stream(), ss);
            std::string body = ss.str();

            try {
                Poco::JSON::Parser parser;
                auto result = parser.parse(body);
                auto root = result.extract<Poco::JSON::Object::Ptr>();

                models::RecordCreateRequest req;
                req.patientId = patientId;
                req.doctorId = payload.value().first;
                req.diagnosis = root->getValue<std::string>("diagnosis");
                req.treatment = root->getValue<std::string>("treatment");
                
                if (root->has("notes")) {
                    req.notes = root->getValue<std::string>("notes");
                } else {
                    req.notes = "";
                }

                auto record = services::PatientService::getInstance().addRecord(req);
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
    }

    utils::JsonResponse::sendError(response, 404, "Endpoint not found");
}

}