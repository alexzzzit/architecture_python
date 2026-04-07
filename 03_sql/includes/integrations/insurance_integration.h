#pragma once
#include <string>
#include <optional>

namespace integrations {

// Структура InsuranceStatus содержит результат проверки страховки пациента
// Возвращается внешним сервисом Insurance System
struct InsuranceStatus {
    bool isValid;           // Действительна ли страховка
    std::string provider;   // Название страховой компании
    std::string expiryDate; // Дата окончания действия страховки
    std::string coverageType; // Тип покрытия (Full, Basic, etc.)
};

// Класс InsuranceIntegration отвечает за взаимодействие с внешней системой проверки страховки
// Соответствует компоненту "Insurance System" в архитектуре
// В текущей реализации работает в mock-режиме для демонстрации
class InsuranceIntegration {
public:
    // Возвращает единственный экземпляр InsuranceIntegration (Singleton паттерн)
    static InsuranceIntegration& getInstance();
    
    // Проверяет действительность страховки по номеру полиса
    // insuranceNumber: номер страхового полиса пациента
    // Возвращает: InsuranceStatus если проверка успешна, иначе nullopt
    std::optional<InsuranceStatus> verifyInsurance(const std::string& insuranceNumber);
    
    // Проверяет, работает ли интеграция в mock-режиме
    // Возвращает: true если mock-режим включён
    bool isMockMode() const;
    
    // Включает или выключает mock-режим
    // enabled: true для mock-режима, false для реальных API вызовов
    void setMockMode(bool enabled);

private:
    // Конструктор инициализирует настройки подключения к внешнему сервису
    InsuranceIntegration();
    
    // Запрещаем копирование экземпляра Singleton
    InsuranceIntegration(const InsuranceIntegration&) = delete;
    InsuranceIntegration& operator=(const InsuranceIntegration&) = delete;
    
    bool _mockMode;           // Флаг mock-режима (true для демонстрации)
    std::string _baseUrl;     // Базовый URL внешнего API страховки
};

}
