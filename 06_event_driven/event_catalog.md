# Каталог событий (Event Catalog)

## MedicalRecordCreated

| Параметр | Описание |
|----------|----------|
| Название события | MedicalRecordCreated |
| Структура payload | record_id (uuid), patient_id (uuid), doctor_id (uuid), diagnosis (string), treatment (string), status (string), visit_date (timestamp) |
| Производитель события | Medical Records API Service |
| Потребители события | SearchIndexService, AnalyticsService, AuditService |
| Гарантии доставки | exactly-once при публикации, at-least-once при потреблении с дедупликацией |

## MedicalRecordUpdated

| Параметр | Описание |
|----------|----------|
| Название события | MedicalRecordUpdated |
| Структура payload | record_id (uuid), updated_fields (список строк), diagnosis (string), treatment (string), updated_at (timestamp) |
| Производитель события | Medical Records API Service |
| Потребители события | SearchIndexService, CacheInvalidationService |
| Гарантии доставки | exactly-once при публикации, at-least-once при потреблении с дедупликацией |

## MedicalRecordStatusChanged

| Параметр | Описание |
|----------|----------|
| Название события | MedicalRecordStatusChanged |
| Структура payload | record_id (uuid), old_status (string), new_status (string), changed_by (uuid), changed_at (timestamp) |
| Производитель события | Medical Records API Service |
| Потребители события | NotificationService, ReportingService, AuditService |
| Гарантии доставки | exactly-once при публикации, at-least-once при потреблении с дедупликацией |

## PatientRegistered

| Параметр | Описание |
|----------|----------|
| Название события | PatientRegistered |
| Структура payload | patient_id (uuid), user_id (uuid), policy_number (string), birth_date (date), phone (string) |
| Производитель события | User Management API Service |
| Потребители события | NotificationService, AnalyticsService |
| Гарантии доставки | exactly-once при публикации, at-least-once при потреблении с дедупликацией |

## DoctorAssignedToRecord

| Параметр | Описание |
|----------|----------|
| Название события | DoctorAssignedToRecord |
| Структура payload | record_id (uuid), doctor_id (uuid), assigned_by (uuid), assigned_at (timestamp) |
| Производитель события | Medical Records API Service |
| Потребители события | NotificationService, SchedulingService |
| Гарантии доставки | exactly-once при публикации, at-least-once при потреблении с дедупликацией |
