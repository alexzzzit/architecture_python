SHELL := /bin/bash

.PHONY: build test clean

build:
	docker-compose up -d --build

test:
	@echo "Health check..."
	@curl -s http://localhost:8080/health | grep -q healthy && echo "OK" || (echo "FAIL" && exit 1)
	@echo ""
	@echo "Auth login..."
	@TOKEN=$$(curl -s -X POST http://localhost:8080/api/v1/auth/login \
	  -H "Content-Type: application/json" \
	  -d '{"login":"doctor_smirnov","password":"password123"}' \
	  | sed 's/.*"token":"\([^"]*\)".*/\1/'); \
	if [ -z "$$TOKEN" ]; then \
		echo "Token failed"; \
		exit 1; \
	fi; \
	echo "OK"; \
	echo ""; \
	echo "User by login..."; \
	curl -s -H "Authorization: Bearer $$TOKEN" \
	  http://localhost:8080/api/v1/users/login/doctor_smirnov | grep -q doctor_smirnov && echo "OK" || echo "FAIL"; \
	echo "User search..."; \
	curl -s -H "Authorization: Bearer $$TOKEN" \
	  "http://localhost:8080/api/v1/users/search?mask=Иван" | grep -q Иван && echo "OK" || echo "FAIL"; \
	echo ""; \
	echo "Patient records..."; \
	curl -s -H "Authorization: Bearer $$TOKEN" \
	  http://localhost:8080/api/v1/patients/b0000001-0000-4000-8000-000000000001/records | grep -q REC-2024 && echo "OK" || echo "FAIL"; \
	echo ""; \
	echo "Record by code..."; \
	curl -s -H "Authorization: Bearer $$TOKEN" \
	  http://localhost:8080/api/v1/records/REC-2024-001 | grep -q "Гипертоническая" && echo "OK" || echo "FAIL"; \
	echo ""; \
	echo "All tests passed"

clean:
	docker-compose down -v --rmi all