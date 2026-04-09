# Домашнее задание №03: Проектирование и оптимизация реляционной базы данных

**Вариант:** 20
**Автор:** Занозин Александр, М8О-107СВ-25

## Схема БД и ключевые значения
База данных состоит из трёх нормализованных таблиц. Связи реализованы через внешние ключи с каскадным удалением или ограничением.

### `users` (Пользователи)

| Поле | Тип | Ограничения | Описание |
|------|-----|-------------|----------|
| id | UUID | PRIMARY KEY | Уникальный идентификатор |
| login | VARCHAR(100) | UNIQUE, NOT NULL | Логин для входа |
| email | VARCHAR(255) | UNIQUE, NOT NULL | Email |
| password_hash | VARCHAR(255) | NOT NULL | Хэш пароля |
| role | user_role (ENUM) | NOT NULL | patient/doctor/admin/lab_technician |
| is_active | BOOLEAN | DEFAULT TRUE | Статус аккаунта |
| created_at, updated_at | TIMESTAMPTZ | DEFAULT CURRENT_TIMESTAMP | Временные метки |

### `patients` (Пациенты)

| Поле | Тип | Ограничения | Описание |
|------|-----|-------------|----------|
| id | UUID | PRIMARY KEY | Уникальный идентификатор |
| user_id | UUID | FK → users(id) CASCADE | Связь с учётной записью |
| policy_number | VARCHAR(50) | UNIQUE, NOT NULL | Номер полиса ОМС |
| snils | VARCHAR(20) | UNIQUE | СНИЛС |
| birth_date | DATE | NOT NULL, CHECK | Дата рождения |
| phone | VARCHAR(20) | | Телефон |
| address | TEXT | | Адрес |

### `medical_records` (Медицинские записи)

| Поле | Тип | Ограничения | Описание |
|------|-----|-------------|----------|
| id | UUID | PRIMARY KEY | Уникальный идентификатор |
| code | VARCHAR(50) | UNIQUE, NOT NULL, CHECK | Уникальный код записи |
| patient_id | UUID | FK → patients(id) CASCADE | Связь с пациентом |
| doctor_id | UUID | FK → users(id) RESTRICT | Связь с врачом |
| diagnosis | TEXT | NOT NULL | Диагноз |
| treatment | TEXT | | Лечение |
| status | record_status (ENUM) | DEFAULT 'draft' | draft/confirmed/archived |
| visit_date | TIMESTAMPTZ | DEFAULT CURRENT_TIMESTAMP | Дата визита |

Автоматическое обновление поля `updated_at` при изменении записей реализовано через триггерную функцию.

## Индексы
Созданы для ускорения операций, которые чаще всего выполняются через API:
- `idx_users_login` (btree) — авторизация и поиск по логину (`WHERE login = $1`)
- `idx_patients_user_id` (btree) — оптимизация JOIN с таблицей пользователей
- `idx_records_patient_visit` (btree) — выборка истории посещений с сортировкой по дате без дополнительного `ORDER BY`
- `idx_records_code` (btree) — прямой доступ к записи по уникальному коду
- `idx_users_name_trgm`, `idx_records_diagnosis_trgm` (GIN + pg_trgm) — ускорение нечёткого поиска по ФИО и диагнозам (`ILIKE '%value%'`)

## Оптимизация запросов
Подробный анализ планов выполнения (`EXPLAIN ANALYZE`), сравнение производительности до и после добавления индексов, переписывание тяжёлых запросов и стратегия партиционирования вынесены в отдельный файл:  
📄 **[sql/optimization.md](sql/optimization.md)**

## Интеграция API и запуск
API на C++ (POCO) подключается к PostgreSQL через `libpq`. Все запросы параметризованы (`$1, $2...`), используется пул соединений и транзакционная целостность при создании связанных сущностей.

**Быстрый старт:**
```bash

make build


make test


make clean
```

Настройки подключения управляются через переменные окружения в `docker-compose.yaml` (`DATABASE_URL`, `JWT_SECRET_KEY`, `PORT`). База данных инициализируется автоматически при первом запуске через скрипты в `docker-entrypoint-initdb.d/`.

