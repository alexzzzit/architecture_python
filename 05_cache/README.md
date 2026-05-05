# Медицинская система записей — Лабораторная работа 05

**Вариант 20:** Система управления медицинскими записями  
**Автор:** Занозин Александр, М8О-107СВ-25  
**Стек:** C++17, POCO, MongoDB 5.0, Docker

## Цель работы
Практическое проектирование системы с учётом производительности: реализация кеширования и rate limiting.

### Сборка
```bash
make build
```

### Запуск тестов

```bash
make test
```

### Сборка + тесты
```bash
make build-tests
```

### Очистка
```bash
make clean
```

### Открыть Swagger UI
```
http://localhost:8081
```

# Реализованные оптимизации
## Кеширование (Cache-Aside)

Применено к GET /patients/search (TTL 300s) и GET /patients/{id}/records (TTL 120s)
Инвалидация выполняется синхронно при POST-запросах
Потокобезопасная in-memory реализация (std::unordered_map + std::mutex)

## Rate Limiting (Fixed Window Counter)

Лимиты: 60 запросов/мин для поиска, 100 для истории записей
При превышении возвращается 429 Too Many Requests с заголовками X-RateLimit-Limit, X-RateLimit-Remaining, X-RateLimit-Reset
Fail-open режим: при ошибке инициализации ограничения снимаются, чтобы не блокировать легитимный трафик

## Соответствие критериям

Обоснованность стратегий кеширования (Cache-Aside для read-heavy операций)
Корректность rate limiting (Fixed Window, HTTP 429 + заголовки)
Качество реализации (потокобезопасность, инвалидация, graceful degradation)
Анализ влияния на производительность (метрики, hit rate, защита пула соединений)


## Новые файлы (оптимизации)
| Файл | Назначение |
|------|-----------|
| `src/cache/CacheService.hpp` | Реализация Cache-Aside: потокобезопасный in-memory кеш с TTL и инвалидацией |
| `src/cache/RateLimiter.hpp` | Реализация Fixed Window Counter: подсчёт запросов, возврат 429, заголовки `X-RateLimit-*` |
| `performance_design.md` | Полное описание стратегии кеширования и rate limiting (анализ, метрики, формулы) |
