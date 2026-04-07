#include "integrations/laboratory_integration.h"
#include "Poco/Environment.h"
#include "Poco/Logger.h"
#include <optional>

namespace integrations {

LaboratoryIntegration::LaboratoryIntegration() 
    : _mockMode(true)
    , _baseUrl(Poco::Environment::get("LAB_API_URL", "https://lab.example.com/api"))
{
    Poco::Logger::get("LaboratoryIntegration").information(
        "Laboratory Integration initialized (mock=%s)", _mockMode ? "true" : "false");
}

LaboratoryIntegration& LaboratoryIntegration::getInstance() {
    static LaboratoryIntegration instance;
    return instance;
}

std::vector<LabResult> LaboratoryIntegration::getPatientResults(const std::string& patientId) {
    if (_mockMode) {
        std::vector<LabResult> results;
        
        LabResult r1;
        r1.testCode = "CBC001";
        r1.testName = "Complete Blood Count";
        r1.result = "Normal";
        r1.unit = "-";
        r1.referenceRange = "Normal";
        r1.performedAt = Poco::Timestamp().epochTime();
        results.push_back(r1);
        
        Poco::Logger::get("LaboratoryIntegration").information(
            "Mock lab results for patient: %s", patientId);
        return results;
    }
    
    Poco::Logger::get("LaboratoryIntegration").warning("Real lab integration not implemented");
    return {};
}

std::optional<LabResult> LaboratoryIntegration::getResultByCode(const std::string& resultCode) {
    if (_mockMode) {
        LabResult result;
        result.testCode = resultCode;
        result.testName = "Mock Test";
        result.result = "Normal";
        result.unit = "-";
        result.referenceRange = "0-100";
        result.performedAt = Poco::Timestamp().epochTime();
        return result;
    }
    
    return std::nullopt;
}

bool LaboratoryIntegration::isMockMode() const {
    return _mockMode;
}

void LaboratoryIntegration::setMockMode(bool enabled) {
    _mockMode = enabled;
}

}