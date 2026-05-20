#pragma once
#include <string>
#include <vector>

namespace services {

// Структура NotificationRequest содержит данные для отправки уведомления
// Используется при создании запроса на отправку email/SMS
struct NotificationRequest {
    std::string userId;       // Идентификатор пользователя-получателя
    std::string email;        // Email адрес для отправки
    std::string phone;        // Телефон для SMS
    std::string subject;      // Тема письма (для email)
    std::string message;      // Текст сообщения
    std::string type;         // Тип уведомления: appointment, record_created, etc.
};

// Структура NotificationResult содержит результат отправки уведомления
// Возвращается после попытки отправки email и/или SMS
struct NotificationResult {
    bool emailSent;           // true если email успешно отправлен
    bool smsSent;             // true если SMS успешно отправлено
    std::string errorMessage; // Текст ошибки если отправка не удалась
};

// Класс NotificationService отвечает за отправку уведомлений пациентам
// Соответствует компоненту "Notification Service" в архитектуре
// Интегрируется с внешними Email Service и SMS Gateway
class NotificationService {
public:
    // Возвращает единственный экземпляр NotificationService (Singleton паттерн)
    static NotificationService& getInstance();
    
    // Отправляет уведомление пользователю (email + SMS)
    // request: структура с данными для отправки
    // Возвращает: NotificationResult с результатами отправки
    NotificationResult sendNotification(const NotificationRequest& request);
    
    // Отправляет email сообщение
    // to: email адрес получателя
    // subject: тема письма
    // body: текст сообщения
    // Возвращает: true если отправка успешна
    bool sendEmail(const std::string& to, const std::string& subject, const std::string& body);
    
    // Отправляет SMS сообщение
    // phone: номер телефона получателя
    // message: текст SMS
    // Возвращает: true если отправка успешна
    bool sendSms(const std::string& phone, const std::string& message);
    
    // Проверяет, работает ли сервис в mock-режиме
    // Возвращает: true если mock-режим включён
    bool isMockMode() const;
    
    // Включает или выключает mock-режим
    // enabled: true для mock-режима, false для реальных вызовов
    void setMockMode(bool enabled);

private:
    // Конструктор инициализирует настройки SMTP и SMS шлюза
    NotificationService();
    
    // Запрещаем копирование экземпляра Singleton
    NotificationService(const NotificationService&) = delete;
    NotificationService& operator=(const NotificationService&) = delete;
    
    bool _mockMode;           // Флаг mock-режима (true для демонстрации)
    std::string _smtpHost;    // SMTP сервер для отправки email
    std::string _smsGatewayUrl; // URL SMS шлюза для отправки SMS
};

}
