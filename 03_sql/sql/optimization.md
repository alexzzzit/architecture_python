# Оптимизация запросов — Medical Records API (Вариант 20)

## Анализ планов выполнения (EXPLAIN ANALYZE)

В ходе проектирования БД были выявлены типовые запросы, которые будут выполняться наиболее часто. Для каждого из них проведён анализ плана выполнения до и после применения индексов. Все замеры выполнены на PostgreSQL 14 на тестовом наборе данных (12 пользователей, 10 пациентов, 15 записей).

---

### 1. Поиск пользователя по логину
**Эндпоинт:** `GET /api/v1/users/login/{login}`  
**Запрос:**
```sql
EXPLAIN ANALYZE
SELECT id, login, first_name, last_name, email, role, is_active
FROM users
WHERE login = 'doctor_smirnov' AND is_active = TRUE;