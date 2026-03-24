#include "integrations/insurance_integration.h"
#include "Poco/Environment.h"
#include "Poco/Logger.h"
#include "Poco/Timestamp.h"

namespace integrations {

// Конструктор инициализирует настройки интеграции
// Получает базовый URL из переменной окружения INSURANCE_API_URL
// По умолчанию работает в mock-режиме для демонстрации
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
// insuranceNumber: номер страхового полиса пациента
// Возвращает: InsuranceStatus если проверка успешна, иначе nullopt
std::optional<InsuranceStatus> InsuranceIntegration::verifyInsurance(const std::string& insuranceNumber) {
    if (_mockMode) {
        // Mock-режим: возвращаем тестовые данные
        InsuranceStatus status;
        status.isValid = true;
        status.provider = "Mock Insurance Co";
        status.expiryDate = "2027-12-31";
        status.coverageType = "Full";
        Poco::Logger::get("InsuranceIntegration").information(
            "Mock insurance verification: %s", insuranceNumber);
        return status;
    }
    
    // TODO: Реальный HTTP вызов к внешней системе страховки
    Poco::Logger::get("InsuranceIntegration").warning("Real insurance verification not implemented");
    return std::nullopt;
}

// Проверяет режим работы интеграции
bool InsuranceIntegration::isMockMode() const {
    return _mockMode;
}

// Включает или выключает mock-режим
void InsuranceIntegration::setMockMode(bool enabled) {
    _mockMode = enabled;
    Poco::Logger::get("InsuranceIntegration").information(
        "Mock mode set to: %s", enabled ? "true" : "false");
}

}
