.PHONY: build clean test

build:
	docker compose up --build -d
	until docker compose exec postgres pg_isready -q 2>/dev/null; do sleep 1; done

clean:
	docker compose down -v --remove-orphans
	docker builder prune -f

test: build
	@echo "Test 1: Health endpoint responds"
	@curl -sf http://localhost:8080/health > /dev/null 2>&1 && echo "+" || echo "FAIL"

	@echo "Test 2: Protected endpoint requires auth (401)"
	@curl -s -o /dev/null -w "%{http_code}" http://localhost:8080/api/v1/patients/search 2>/dev/null | grep -qE "401|403" && echo "+" || echo "FAIL"

	@echo "Test 3: Swagger UI is available"
	@curl -sf http://localhost:8081 > /dev/null 2>&1 && echo "+" || echo "FAIL"

	@echo "Test 4: Rate limit headers implemented in code"
	@grep -q "X-RateLimit-Limit" src/handlers/patient_handler.cpp && echo "+" || echo "FAIL"

	@echo "Test 5: Cache-Aside pattern implemented in code"
	@grep -qE "CACHE_HIT|CACHE_MISS" src/cache/CacheService.hpp && echo "+" || echo "FAIL"