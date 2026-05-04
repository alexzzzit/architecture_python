#include "handlers/router_factory.h"
#include "handlers/auth_handler.h"
#include "handlers/user_handler.h"
#include "handlers/patient_handler.h"
#include "handlers/record_handler.h"
#include <Poco/Net/HTTPRequestHandler.h>
#include <Poco/Net/HTTPServerRequest.h>
#include <Poco/Net/HTTPServerResponse.h>
#include <Poco/Logger.h>

using namespace Poco::Net;

namespace handlers {

HTTPRequestHandler* RouterFactory::createRequestHandler(const HTTPServerRequest& request) {
    auto& logger = Poco::Logger::get("Router");
    const std::string& path = request.getURI();

    logger.debug("Routing request: %s %s", request.getMethod(), path);

    // Health check
    if (path == "/health" || path == "/api/health") {
        return new HealthHandler();
    }

    // Swagger spec
    if (path == "/swagger.yaml" || path == "/api/swagger.yaml") {
        return new SwaggerHandler();
    }

    // Auth endpoints
    if (path == "/api/v1/auth/login" && request.getMethod() == HTTPRequest::HTTP_POST) {
        return new AuthHandler();
    }
    if (path == "/api/v1/auth/logout" && request.getMethod() == HTTPRequest::HTTP_POST) {
        return new AuthHandler();
    }

    // Users
    if (path == "/api/v1/users" && request.getMethod() == HTTPRequest::HTTP_GET) {
        return new UserHandler();
    }
    if (path == "/api/v1/users" && request.getMethod() == HTTPRequest::HTTP_POST) {
        return new UserHandler();
    }

    // Patients
    if (path == "/api/v1/patients" && request.getMethod() == HTTPRequest::HTTP_GET) {
        return new PatientHandler();
    }
    if (path == "/api/v1/patients" && request.getMethod() == HTTPRequest::HTTP_POST) {
        return new PatientHandler();
    }

    // Records
    if (path == "/api/v1/records" && request.getMethod() == HTTPRequest::HTTP_GET) {
        return new RecordHandler();
    }
    if (path == "/api/v1/records" && request.getMethod() == HTTPRequest::HTTP_POST) {
        return new RecordHandler();
    }

    // Not found
    return new NotFoundHandler();
}

} // namespace handlers
