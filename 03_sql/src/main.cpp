#include "Poco/Net/HTTPServer.h"
#include "Poco/Net/ServerSocket.h"
#include "Poco/Net/HTTPRequestHandlerFactory.h"
#include "Poco/Net/HTTPRequestHandler.h"
#include "Poco/Util/ServerApplication.h"
#include "Poco/Logger.h"
#include "Poco/AutoPtr.h"
#include "Poco/Environment.h"
#include <iostream>

#include "handlers/user_handler.h"
#include "handlers/patient_handler.h"
#include "handlers/record_handler.h"
#include "handlers/auth_handler.h"

using namespace Poco::Net;
using namespace Poco::Util;

class MedicalRecordsHandlerFactory : public HTTPRequestHandlerFactory {
public:
    HTTPRequestHandler* createRequestHandler(const HTTPServerRequest& request) override {
        const std::string& uri = request.getURI();
        const std::string& method = request.getMethod();

        Poco::Logger::get("MedicalRecords").information("Request: %s %s", method, uri);

        if (uri.find("/api/v1/auth") == 0) {
            return new handlers::AuthHandler();
        }
        if (uri.find("/api/v1/users") == 0) {
            return new handlers::UserHandler();
        }
        if (uri.find("/api/v1/patients") == 0) {
            return new handlers::PatientHandler();
        }
        if (uri.find("/api/v1/records") == 0) {
            return new handlers::RecordHandler();
        }
        if (uri == "/health" || uri == "/api/v1/health") {
            return new handlers::HealthHandler();
        }
        if (uri == "/swagger.yaml" || uri == "/openapi.yaml") {
            return new handlers::SwaggerHandler();
        }

        return new handlers::NotFoundHandler();
    }
};

class MedicalRecordsApp : public ServerApplication {
protected:
    void initialize(Application& self) override {
        loadConfiguration();
        ServerApplication::initialize(self);
    }

    int main(const std::vector<std::string>& args) override {
        std::string portStr = Poco::Environment::get("PORT", "8080");
        uint16_t port = static_cast<uint16_t>(std::stoi(portStr));

        ServerSocket socket(port);
        HTTPServer server(new MedicalRecordsHandlerFactory(), socket, new HTTPServerParams);

        Poco::Logger::get("MedicalRecords").information("========================================");
        Poco::Logger::get("MedicalRecords").information("Medical Records API starting");
        Poco::Logger::get("MedicalRecords").information("Version: %s", "1.0.0");
        Poco::Logger::get("MedicalRecords").information("Port: %d", port);
        Poco::Logger::get("MedicalRecords").information("========================================");
        
        server.start();

        waitForTerminationRequest();
        
        Poco::Logger::get("MedicalRecords").information("Shutting down...");
        server.stopAll();

        return Application::EXIT_OK;
    }
};

POCO_SERVER_MAIN(MedicalRecordsApp)