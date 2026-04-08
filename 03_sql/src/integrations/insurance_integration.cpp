#include "integrations/insurance_integration.h"
#include "Poco/Environment.h"
#include "Poco/Logger.h"
#include "Poco/Timestamp.h"

namespace integrations {

// Конструктор инициализирует настройки интеграции
InsuranceIntegration::InsuranceIntegration() 
    : _mockMode(true)
    , _baseUrl(Poco::Environment::get("INSURANCE_API_URL", "https://insurance.example.com/api"))
{
    Poco::Logger::get("InsuranceIntegration").information(
        "Insurance Integration initialized (mock=%s)", _mockMode ? "true" : "false");
}

// Возвращает единственный экземпляр InsuranceIntegration
InsuranceIntegration& InsuranceIntegration::getInstance() {
    static InsuranceIntegration instance;
    return instance;
}

// Проверяет действительность страховки по номеру полиса
std::optional<InsuranceStatus> InsuranceIntegration::verifyInsurance(const std::string& insuranceNumber) {
    if (_mockMode) {
        InsuranceStatus status;
        status.isValid = true;
        status.provider = "Mock Insurance Co";
        status.expiryDate = "2027-12-31";
        status.coverageType = "Full";
        Poco::Logger::get("InsuranceIntegration").information(
            "Mock insurance verification: %s", insuranceNumber);
        return status;
    }
    
    Poco::Logger::get("InsuranceIntegration").warning("Real insurance verification not implemented");
    return std::nullopt;
}

bool InsuranceIntegration::isMockMode() const {
    return _mockMode;
}

void InsuranceIntegration::setMockMode(bool enabled) {
    _mockMode = enabled;
    Poco::Logger::get("InsuranceIntegration").information(
        "Mock mode set to: %s", enabled ? "true" : "false");
}

}
