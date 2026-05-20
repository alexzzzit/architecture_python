#include "services/notification_service.h"
#include "Poco/Environment.h"
#include "Poco/Logger.h"

namespace services {

// Конструктор инициализирует настройки сервиса уведомлений
// Получает SMTP хост и SMS шлюз из переменных окружения
// По умолчанию работает в mock-режиме для демонстрации
NotificationService::NotificationService() 
    : _mockMode(true)
    , _smtpHost(Poco::Environment::get("SMTP_HOST", "smtp.example.com"))
    , _smsGatewayUrl(Poco::Environment::get("SMS_GATEWAY_URL", "https://sms.example.com/api"))
{
    Poco::Logger::get("NotificationService").information(
        "Notification Service initialized (mock=%s)", _mockMode ? "true" : "false");
}

// Возвращает единственный экземпляр NotificationService
NotificationService& NotificationService::getInstance() {
    static NotificationService instance;
    return instance;
}

// Отправляет уведомление пользователю (email + SMS)
// request: структура с данными для отправки
// Возвращает: NotificationResult с результатами отправки
NotificationResult NotificationService::sendNotification(const NotificationRequest& request) {
    NotificationResult result;
    result.emailSent = sendEmail(request.email, request.subject, request.message);
    result.smsSent = sendSms(request.phone, request.message);
    return result;
}

// Отправляет email сообщение
// to: email адрес получателя
// subject: тема письма
// body: текст сообщения
// Возвращает: true если отправка успешна
bool NotificationService::sendEmail(const std::string& to, const std::string& subject, const std::string& body) {
    if (_mockMode) {
        // Mock-режим: логируем отправку
        Poco::Logger::get("NotificationService").information(
            "Mock email sent to: %s, subject: %s", to, subject);
        return true;
    }
    
    // TODO: Реальная SMTP отправка через POCO Net
    Poco::Logger::get("NotificationService").warning("Real email sending not implemented");
    return false;
}

// Отправляет SMS сообщение
// phone: номер телефона получателя
// message: текст SMS
// Возвращает: true если отправка успешна
bool NotificationService::sendSms(const std::string& phone, const std::string& message) {
    if (_mockMode) {
        // Mock-режим: логируем отправку
        Poco::Logger::get("NotificationService").information(
            "Mock SMS sent to: %s", phone);
        return true;
    }
    
    // TODO: Реальный HTTP вызов к SMS шлюзу
    Poco::Logger::get("NotificationService").warning("Real SMS sending not implemented");
    return false;
}

// Проверяет режим работы сервиса
bool NotificationService::isMockMode() const {
    return _mockMode;
}

// Включает или выключает mock-режим
void NotificationService::setMockMode(bool enabled) {
    _mockMode = enabled;
}

}
