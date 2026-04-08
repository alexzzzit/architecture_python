# Оптимизация запросов — Medical Records API (Вариант 20)

## 📊 Анализ планов выполнения (EXPLAIN ANALYZE)

### 1. Поиск пользователя по логину

**Запрос:**
```sql
EXPLAIN ANALYZE
SELECT id, login, first_name, last_name, email, role
FROM users
WHERE login = 'doctor_smirnov' AND is_active = true;