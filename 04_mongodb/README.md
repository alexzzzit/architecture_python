```markdown
# Медицинская система записей — MongoDB
## Домашнее задание 04

**Вариант 20**: Система управления медицинскими записями  
**Автор**: Занозин Александр, М8О-107СВ-25  
**Стек**: C++17, POCO, MongoDB 5.0, Docker

---

## Описание проекта

Backend-сервис для работы с медицинскими записями пациентов. Реализует:
- Аутентификацию пользователей (врачи, пациенты)
- Просмотр и поиск пациентов по ФИО
- Создание и получение медицинских записей
- Валидацию данных через `$jsonSchema`
- REST API с авторизацией по JWT

Данные хранятся в MongoDB в трёх коллекциях: `users`, `patients`, `medical_records`.

---

## Запуск

```bash
# 1. Собрать и запустить контейнеры
docker compose up --build -d
docker compose exec mongodb mongosh medical_db < data.js
curl http://localhost:8080/health
```

### Остановка
```bash
docker compose down
```

---

## Выполнение запросов

### Подключение к MongoDB
```bash
docker compose exec mongodb mongosh medical_db

docker compose exec mongodb mongosh medical_db < queries.js
```

### Запуск тестов
```bash
make test
```

---

## Файлы проекта

| Файл | Назначение |
|------|-----------|
| `data.js` | Тестовые данные (10+ документов на коллекцию) |
| `queries.js` | CRUD-операции с операторами MongoDB |
| `validation.js` | Валидация схем через `$jsonSchema` |
| `schema_design.md` | Описание документной модели |
| `Dockerfile`, `docker-compose.yaml` | Сборка и запуск в Docker |
| `src/main.cpp` | Исходный код API на C++/POCO |
