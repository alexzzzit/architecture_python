workspace "Medical Records System" "Система управления медицинскими записями" {

    model {
        properties {
            structurizr.groupSeparator "/"
            workspace_cmdb "med_records"
            architect "alex"
        }

        patient = person "Пациент" "Получает медицинские услуги"
        doctor = person "Врач" "Создаёт и просматривает медицинские записи"
        admin = person "Администратор" "Управляет пользователями и справочниками"
        labTechnician = person "Лаборант" "Добавляет результаты анализов"

        emailService = softwareSystem "Email Service" "Внешний сервис email-рассылки"
        smsGateway = softwareSystem "SMS Gateway" "Внешний сервис SMS-уведомлений"
        insuranceSystem = softwareSystem "Insurance System" "Система проверки страховки"
        labSystem = softwareSystem "Laboratory System" "Лабораторная информационная система"

        medSystem = softwareSystem "Medical Records System" "Система управления медицинскими записями" {

            patientWeb = container "Patient Web Portal" "Веб-приложение для пациентов" "HTML, JavaScript, CSS"
            doctorWeb = container "Doctor Web Portal" "Веб-приложение для врачей" "HTML, JavaScript, CSS"
            adminWeb = container "Admin Web Portal" "Веб-приложение для администраторов" "HTML, JavaScript, CSS"

            apiGateway = container "API Gateway" "Единая точка входа для всех API запросов" "Nginx"

            userService = container "User Service" "Сервис управления пользователями" "Java, Spring Boot"
            patientService = container "Patient Service" "Сервис управления пациентами" "Java, Spring Boot"
            recordsService = container "Medical Records Service" "Сервис медицинских записей" "Java, Spring Boot"
            notificationService = container "Notification Service" "Сервис уведомлений (Email, SMS)" "PHP"

            userDb = container "User Database" "Хранение данных пользователей" "PostgreSQL 14"
            patientDb = container "Patient Database" "Хранение данных пациентов" "PostgreSQL 14"
            recordsDb = container "Medical Records Database" "Хранение медицинских записей" "PostgreSQL 14"
            cache = container "Cache Server" "Кеширование частых запросов" "Redis"

            patient -> patientWeb "Регистрация, просмотр записей" "HTTPS:443"
            doctor -> doctorWeb "Создание и просмотр записей" "HTTPS:443"
            admin -> adminWeb "Управление пользователями" "HTTPS:443"
            labTechnician -> doctorWeb "Добавление результатов анализов" "HTTPS:443"

            patientWeb -> apiGateway "REST API запросы" "HTTPS:443"
            doctorWeb -> apiGateway "REST API запросы" "HTTPS:443"
            adminWeb -> apiGateway "REST API запросы" "HTTPS:443"

            apiGateway -> userService "Маршрутизация запросов" "HTTPS:443"
            apiGateway -> patientService "Маршрутизация запросов" "HTTPS:443"
            apiGateway -> recordsService "Маршрутизация запросов" "HTTPS:443"
            apiGateway -> notificationService "Маршрутизация запросов" "HTTPS:443"

            userService -> userDb "CRUD операции" "JDBC:5432"
            patientService -> patientDb "CRUD операции" "JDBC:5432"
            recordsService -> recordsDb "CRUD операции" "JDBC:5432"

            userService -> cache "Кеширование данных пользователей" "Redis:6379"
            patientService -> cache "Кеширование данных пациентов" "Redis:6379"
            recordsService -> cache "Кеширование медицинских записей" "Redis:6379"

            userService -> notificationService "Отправка уведомлений" "HTTPS:443"
            patientService -> notificationService "Отправка уведомлений" "HTTPS:443"
            recordsService -> notificationService "Отправка уведомлений" "HTTPS:443"

            patientService -> userService "Получение данных пользователя" "HTTPS:443"
            recordsService -> patientService "Получение данных пациента" "HTTPS:443"

            notificationService -> emailService "Отправка email" "SMTP:25"
            notificationService -> smsGateway "Отправка SMS" "HTTPS:443"
            patientService -> insuranceSystem "Проверка страховки" "HTTPS:443"
            recordsService -> labSystem "Получение результатов анализов" "HTTPS:443"
        }
    }

    views {
        properties {
            plantuml.format "svg"
            structurizr.sort "created"
            structurizr.tooltips "true"
        }

        themes default

        # C1 System Context Diagram
        systemContext medSystem "system-context" {
            include *
            autoLayout
        }

        # C2 Container Diagram
        container medSystem "container-diagram" {
            include *
            autoLayout
        }

        # Dynamic Diagram - Создание медицинской записи
        dynamic medSystem "dynamic-create-record" "Создание записи" {
            doctor -> doctorWeb "1" "HTTPS:443"
            doctorWeb -> apiGateway "2" "HTTPS:443"
            apiGateway -> recordsService "3" "HTTPS:443"
            recordsService -> recordsDb "4" "JDBC:5432"
        }
    }
}