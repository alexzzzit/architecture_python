.PHONY: build test clean

BUILD_DIR := build

# Сборка проекта
build:
	@mkdir -p $(BUILD_DIR)
	@cd $(BUILD_DIR) && cmake .. -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTS=OFF && make -j$$(nproc)

# Сборка с тестами
build-tests:
	@mkdir -p $(BUILD_DIR)
	@cd $(BUILD_DIR) && cmake .. -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTS=ON && make -j$$(nproc)

# Запуск тестов
test: build-tests
	@cd $(BUILD_DIR) && ./medical_records_tests --gtest_color=yes

# Очистка
clean:
	@rm -rf $(BUILD_DIR)/
	@find . -name "*.o" -delete 2>/dev/null || true
	@find . -name "*.a" -delete 2>/dev/null || true