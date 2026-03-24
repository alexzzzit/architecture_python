.PHONY: all build build-tests test clean distclean docker docker-up docker-down docker-logs open help run status check-deps

BUILD_DIR := build
PROJECT_NAME := medical_records_api
DOCKER_COMPOSE := docker-compose

# Основная цель
all: build

# Сборка проекта
build:
	@mkdir -p $(BUILD_DIR)
	@cd $(BUILD_DIR) && cmake .. -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTS=OFF && make -j$$(nproc)

# Сборка с тестами
build-tests:
	@mkdir -p $(BUILD_DIR)
	@cd $(BUILD_DIR) && cmake .. -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTS=ON && make -j$$(nproc)

# Запуск тестов (только логи Google Test)
test: build-tests
	@cd $(BUILD_DIR) && ./medical_records_tests --gtest_color=yes

# Docker
docker: docker-down
	@$(DOCKER_COMPOSE) build

docker-up: docker
	@$(DOCKER_COMPOSE) up -d
	@$(DOCKER_COMPOSE) ps

docker-down:
	@$(DOCKER_COMPOSE) down 2>/dev/null || true

docker-logs:
	@$(DOCKER_COMPOSE) logs -f

docker-restart: docker-down docker-up

# Открыть Swagger UI
open:
	echo "Откройте вручную: http://localhost:8081";

# Запуск всего
run: build docker-up open

# Очистка
clean:
	@rm -rf $(BUILD_DIR)/
	@find . -name "*.o" -delete 2>/dev/null || true
	@find . -name "*.a" -delete 2>/dev/null || true

distclean: clean docker-down
	@docker rmi $$(docker images -q --filter reference=medical-records-*) 2>/dev/null || true
	@docker builder prune -f 2>/dev/null || true

# Проверка зависимостей
check-deps:
	@command -v cmake &> /dev/null && echo "✓ cmake: $$(cmake --version | head -1)" || echo "✗ cmake не найден"
	@command -v g++ &> /dev/null && echo "✓ g++: $$(g++ --version | head -1)" || echo "✗ g++ не найден"
	@command -v docker &> /dev/null && echo "✓ docker: $$(docker --version)" || echo "✗ docker не найден"
	@command -v docker-compose &> /dev/null && echo "✓ docker-compose: $$(docker-compose --version)" || echo "✗ docker-compose не найден"

# Статус
status:
	@$(DOCKER_COMPOSE) ps 2>/dev/null || echo "Docker не запущен"
	@test -f "$(BUILD_DIR)/$(PROJECT_NAME)" && echo "✓ Бинарник собран" || echo "✗ Бинарник не найден"
	@test -f "$(BUILD_DIR)/medical_records_tests" && echo "✓ Тесты собраны" || echo "✗ Тесты не собраны"

# Справка
help:
	@echo "Medical Records API - Makefile"
	@echo ""
	@echo "Команды:"
	@echo "  make          - Сборка проекта"
	@echo "  make build    - Сборка проекта"
	@echo "  make build-tests - Сборка с тестами"
	@echo "  make test     - Запуск тестов (Google Test логи)"
	@echo "  make docker-up   - Запуск Docker"
	@echo "  make docker-down - Остановка Docker"
	@echo "  make open     - Открыть Swagger UI"
	@echo "  make run      - Сборка + Docker + Открыть"
	@echo "  make clean    - Очистка"
	@echo "  make distclean - Полная очистка"
	@echo "  make help     - Эта справка"