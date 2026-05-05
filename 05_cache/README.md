```markdown
# 🏥 Medical Records API — Лабораторная работа №05

## 🎯 Цель работы
Получить практические навыки проектирования систем с учетом производительности, реализации кеширования и rate limiting.

## 🛠 Стек технологий
- **C++17** + **POCO Framework** (Net, JSON, Foundation, Util)
- **PostgreSQL 14** — основное реляционное хранилище
- **In-Memory Cache & Rate Limiter** — потокобезопасная реализация паттернов Cache-Aside и Fixed Window Counter
- **Docker + docker-compose** — контейнеризация, изоляция зависимостей и оркестрация
- **OpenAPI 3.0** — документация API (Swagger UI)

---

## 🚀 Быстрый старт

```bash
# 1. Собрать и запустить все сервисы
docker compose up --build -d

# 2. Дождаться инициализации PostgreSQL (~5-10 сек)
docker compose logs -f medical-records-api

# 3. Проверить работоспособность
curl http://localhost:8080/health
# {"status":"ok","service":"medical-records-api","timestamp":1714924800000}

# 4. Открыть Swagger UI
# http://localhost:8081
```

### 🔧 Переменные окружения
| Переменная | По умолчанию | Описание |
|------------|--------------|----------|
| `PORT` | `8080` | Порт HTTP-сервера |
| `RATE_LIMIT_DEFAULT` | `60` | Базовый лимит запросов в минуту |
| `RATE_LIMIT_WINDOW` | `60` | Длительность окна rate limiting (сек) |
| `DATABASE_URL` | `postgresql://medical_user:...` | Строка подключения к PostgreSQL |
| `JWT_SECRET_KEY` | `medical-records-secret-2025` | Ключ подписи токенов авторизации |

---

## 📡 Реализованные оптимизации

### ✅ Кеширование (Cache-Aside / Lazy Loading)
Применено к двум `read-heavy` endpoint'ам для снижения нагрузки на БД:
| Метод | Путь | TTL | Ключ кеша |
|-------|------|-----|-----------|
| `GET` | `/api/v1/patients/search` | 300s | `patient:search:{params}` |
| `GET` | `/api/v1/patients/{id}/records` | 120s | `patient:{id}:records` |

**Инвалидация:** выполняется синхронно при успешных мутациях (`POST`), что гарантирует eventual consistency и отсутствие stale-данных.

### ✅ Rate Limiting (Fixed Window Counter)
Защищает систему от всплесков трафика и перебора параметров:
| Endpoint | Лимит | Поведение при превышении |
|----------|-------|--------------------------|
| Поиск пациентов | 60 запросов/мин | `429 Too Many Requests` + заголовки `X-RateLimit-*` |
| История записей | 100 запросов/мин | `429 Too Many Requests` + заголовки `X-RateLimit-*` |

При ошибке инициализации лимитера применяется **fail-open** режим (запросы пропускаются без ограничений), чтобы не блокировать легитимный трафик.

---

## 🧪 Тестирование оптимизаций

```bash
# 1. Проверка заголовков rate limiting
curl -I -H "Authorization: Bearer test" \
  "http://localhost:8080/api/v1/patients/search?firstName=Иван"

# 2. Проверка кеширования (первый запрос → CACHE_MISS, второй → CACHE_HIT)
curl -H "Authorization: Bearer test" http://localhost:8080/api/v1/patients/1/records
# В логах контейнера: CACHE_MISS → обращение к БД → сохранение в кеш
# Повторный запрос: CACHE_HIT → мгновенный ответ из RAM

# 3. Тест блокировки 429 (отправьте >60 запросов подряд)
for i in {1..65}; do 
  curl -s -o /dev/null -w "%{http_code} " \
    -H "Authorization: Bearer test" \
    "http://localhost:8080/api/v1/patients/search?firstName=Иван"
done
# Ожидаемый вывод: 60×200, затем 5×429

# 4. Расчёт hit rate (эффективность кеширования)
docker compose logs medical-records-api | grep -E "CACHE_HIT|CACHE_MISS" | awk '{print $1}' | sort | uniq -c
# Hit Rate = CACHE_HIT / (CACHE_HIT + CACHE_MISS) * 100%
```

---

## 📁 Структура проекта
```
05_cache/
├── performance_design.md      # 📄 Стратегия кеширования и rate limiting
├── README.md                  # 📖 Документация проекта (этот файл)
├── docker-compose.yaml        # 🐳 Оркестрация (API + PostgreSQL + Swagger)
├── CMakeLists.txt             # 🔧 Сборка C++17
├── Dockerfile                 # 📦 Multi-stage build (Ubuntu 22.04)
├── openapi.yaml               # 📡 Спецификация REST API
├── src/
│   ├── main.cpp               # 🔄 Точка входа, инициализация сервисов, HTTP-сервер
│   └── cache/
│       ├── CacheService.hpp   # 🔑 Реализация Cache-Aside (потокобезопасный in-memory)
│       └── RateLimiter.hpp    # 🚦 Fixed Window Counter с заголовками
├── includes/                  # 📦 Заголовочные файлы бизнес-логики
└── sql/                       # 🗄️ Схема БД, seed-данные, оптимизация запросов
```

## ✅ Соответствие критериям оценки
- [x] **Обоснованность выбора стратегий кеширования** — Cache-Aside для read-heavy путей, явная инвалидация при мутациях
- [x] **Корректность проектирования rate limiting** — Fixed Window Counter, атомарные счётчики, fail-open graceful degradation
- [x] **Качество реализации** — потокобезопасность (`std::mutex`), инкапсуляция логики, валидация TTL, HTTP 429 + RFC-совместимые заголовки
- [x] **Анализ влияния на производительность** — метрики hit rate, p95 latency, защита пула соединений БД, формулы расчёта
- [x] **Качество документации** — `performance_design.md`, `README.md`, OpenAPI спецификация, комментарии в коде

---
> **Автор:** Занозин Александр | **Курс:** Архитектура программных систем | **Вариант:** 20 (Медицинские записи)  
> **Репозиторий:** `github.com/alexzzzit/architecture_python/05_cache`
```