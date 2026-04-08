
### Таблицы

| Таблица | Описание | Записей (тест) |
|---------|----------|----------------|
| `users` | Пользователи системы (пациенты, врачи, админы) | 12 |
| `patients` | Расширенная информация о пациентах | 10 |
| `medical_records` | Медицинские записи | 15 |

### Ключевые ограничения

- `users.login`, `users.email` — `UNIQUE NOT NULL`
- `patients.policy_number`, `patients.snils` — `UNIQUE`
- `medical_records.code` — `UNIQUE NOT NULL` (формат: `REC-YYYY-NNN`)
- `role` — ENUM: `patient`, `doctor`, `admin`, `lab_technician`
- `status` — ENUM: `draft`, `confirmed`, `archived`
- `CHECK`: длина login ≥ 3, формат code, birth_date ≤ CURRENT_DATE

### Индексы

| Индекс | Назначение |
|--------|------------|
| `idx_users_login` | Поиск по логину (авторизация) |
| `idx_users_name_trgm` | Нечёткий поиск по ФИО (триграммы) |
| `idx_patients_user_id` | JOIN с users |
| `idx_patients_policy` | Поиск по номеру полиса |
| `idx_records_code` | Быстрый поиск записи по коду |
| `idx_records_patient_visit` | История записей пациента + сортировка |
| `idx_records_diagnosis_trgm` | Поиск по диагнозу |

---

## 🚀 Быстрый старт

### Требования
- Docker ≥ 20.10
- Docker Compose ≥ 2.0

### Запуск инфраструктуры

```bash
# 1. Клонировать репозиторий
git clone <your-repo>
cd 03_sql

# 2. Запустить контейнеры (БД + API + Swagger)
docker-compose up -d

# 3. Проверить статус
docker-compose ps
# Ожидаемо: все контейнеры в статусе "Up (healthy)"

# 4. Проверить данные в БД
docker exec -it medical-postgres psql -U medical_user -d medical_records -c "\dt"
docker exec -it medical-postgres psql -U medical_user -d medical_records -c "SELECT COUNT(*) FROM users, patients, medical_records;"



------------------------
# 🗄️ Управление и проверка PostgreSQL

# 1. Запуск и остановка инфраструктуры
docker-compose up -d          # Запустить API, БД и Swagger в фоне
docker-compose down -v        # Остановить и удалить все контейнеры + volume с данными БД

# 2. Проверка статуса и логов
docker-compose ps             # Статус: medical-postgres должен быть (healthy)
docker-compose logs postgres --tail=20  # Логи БД (ищи строку "database system is ready to accept connections")

# 3. Проверка схемы таблиц
docker exec -it medical-postgres psql -U medical_user -d medical_records -c "\dt"
# Ожидаемый вывод: public | medical_records | public | patients | public | users

# 4. Проверка количества тестовых данных
docker exec -it medical-postgres psql -U medical_user -d medical_records -c "SELECT 'users' as table_name, COUNT(*) FROM users UNION ALL SELECT 'patients', COUNT(*) FROM patients UNION ALL SELECT 'records', COUNT(*) FROM medical_records;"
# Ожидаемый вывод: users=12, patients=10, records=15

# 5. Интерактивная консоль psql
docker exec -it medical-postgres psql -U medical_user -d medical_records
# Внутри консоли доступны команды:
# \dt              # показать таблицы
# \d users         # структура таблицы
# SELECT * FROM users LIMIT 3;  # просмотр данных
# \q               # выход из консоли

# 6. Ручное применение SQL-скриптов (обновление схемы/данных)
docker exec -i medical-postgres psql -U medical_user -d medical_records < sql/schema.sql
docker exec -i medical-postgres psql -U medical_user -d medical_records < sql/data.sql

# 7. Полный сброс БД (если контейнер падает с ошибкой инициализации)
docker-compose down -v
docker volume prune -f
docker-compose up -d

----------------------------------
# === Health check ===
curl http://localhost:8080/health

# === Авторизация ===
TOKEN=$(curl -s -X POST http://localhost:8080/api/v1/auth/login \
  -H "Content-Type: application/json" \
  -d '{"login":"doctor_smirnov","password":"password123"}' \
  | sed 's/.*"token":"\([^"]*\)".*/\1/')

# === Пользователи ===
curl -H "Authorization: Bearer $TOKEN" \
  http://localhost:8080/api/v1/users/login/doctor_smirnov

curl -H "Authorization: Bearer $TOKEN" \
  "http://localhost:8080/api/v1/users/search?mask=Иван"

# === Пациенты ===
curl -H "Authorization: Bearer $TOKEN" \
  http://localhost:8080/api/v1/patients/b0000001-0000-4000-8000-000000000001/records

# === Медицинские записи ===
curl -H "Authorization: Bearer $TOKEN" \
  http://localhost:8080/api/v1/records/REC-2024-001

# === Swagger UI ===
# Открой в браузере: http://localhost:8081