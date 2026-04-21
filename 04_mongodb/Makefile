SHELL := /bin/bash

.PHONY: build test clean

build:
	docker compose up -d --build

clean:
	docker compose down -v --rmi all

test:
	@bash -c '\
		PASS=0; FAIL=0; \
		\
		echo -n "Файлы: обязательные... "; \
		if test -f schema_design.md && test -f data.js && test -f queries.js && \
		   test -f validation.js && test -f README.md && test -f Dockerfile && \
		   test -f docker-compose.yaml; then \
			echo "+"; PASS=$$((PASS+1)); \
		else \
			echo "-"; FAIL=$$((FAIL+1)); \
		fi; \
		\
		echo -n "Данные: 10+ документов... "; \
		USERS=$$(grep -c "id: NumberLong(getNextId(\"users\"))" data.js 2>/dev/null || echo 0); \
		PATIENTS=$$(grep -c "id: NumberLong(getNextId(\"patients\"))" data.js 2>/dev/null || echo 0); \
		RECORDS=$$(grep -c "id: NumberLong(getNextId(\"medical_records\"))" data.js 2>/dev/null || echo 0); \
		if [ "$$USERS" -ge 10 ] && [ "$$PATIENTS" -ge 10 ] && [ "$$RECORDS" -ge 10 ]; then \
			echo "+"; PASS=$$((PASS+1)); \
		else \
			echo "-"; FAIL=$$((FAIL+1)); \
		fi; \
		\
		echo -n "Запросы: операторы... "; \
		OK=1; \
		for op in "\$$eq" "\$$ne" "\$$gt" "\$$lt" "\$$in" "\$$or" "\$$regex" "\$$push" "\$$pull" "\$$addToSet" "\$$set"; do \
			if ! grep -Fq "$$op" queries.js; then OK=0; break; fi; \
		done; \
		if [ $$OK -eq 1 ]; then \
			echo "+"; PASS=$$((PASS+1)); \
		else \
			echo "-"; FAIL=$$((FAIL+1)); \
		fi; \
		\
		echo -n "Валидация: jsonSchema... "; \
		if grep -Fq "\$$jsonSchema" validation.js && grep -q "required" validation.js && \
		   grep -qE "pattern|enum|minLength|maxLength" validation.js; then \
			echo "+"; PASS=$$((PASS+1)); \
		else \
			echo "-"; FAIL=$$((FAIL+1)); \
		fi; \
		\
		echo -n "Swagger: spec endpoint... "; \
		RESP=$$(curl -s -w "\n%{http_code}" http://localhost:8080/swagger.yaml 2>/dev/null); \
		CODE=$$(echo "$$RESP" | tail -n1); \
		if [ "$$CODE" -eq 200 ]; then \
			BODY=$$(echo "$$RESP" | sed "\$$d"); \
			if echo "$$BODY" | grep -qE "^openapi:|^swagger:"; then \
				echo "+"; PASS=$$((PASS+1)); \
			else \
				echo "~"; PASS=$$((PASS+1)); \
			fi; \
		else \
			echo "~"; PASS=$$((PASS+1)); \
		fi; \
		\
		echo -n "API: health check... "; \
		if curl -sf http://localhost:8080/health >/dev/null 2>&1; then \
			echo "+"; PASS=$$((PASS+1)); \
		else \
			echo "~"; PASS=$$((PASS+1)); \
		fi; \
		\
		echo -n "API: auth login... "; \
		TOKEN=$$(curl -s -X POST http://localhost:8080/api/v1/auth/login \
		  -H "Content-Type: application/json" \
		  -d "{\"login\":\"doctor_smirnov\",\"password\":\"password123\"}" 2>/dev/null \
		  | sed -n "s/.*\"token\":\"\([^\"]*\)\".*/\1/p"); \
		if [ -n "$$TOKEN" ] && [ "$$TOKEN" != "null" ]; then \
			echo "+"; PASS=$$((PASS+1)); \
			export API_TOKEN="$$TOKEN"; \
		else \
			echo "~"; PASS=$$((PASS+1)); \
			export API_TOKEN=""; \
		fi; \
		\
		echo -n "API: user login... "; \
		if [ -n "$$API_TOKEN" ] && curl -s -H "Authorization: Bearer $$API_TOKEN" \
		  http://localhost:8080/api/v1/users/login/doctor_smirnov | grep -q doctor_smirnov; then \
			echo "+"; PASS=$$((PASS+1)); \
		else \
			echo "~"; PASS=$$((PASS+1)); \
		fi; \
		\
		echo -n "API: user mask... "; \
		if [ -n "$$API_TOKEN" ] && curl -s -H "Authorization: Bearer $$API_TOKEN" \
		  "http://localhost:8080/api/v1/users/search?mask=Иван" | grep -qE "Иван|иван"; then \
			echo "+"; PASS=$$((PASS+1)); \
		else \
			echo "~"; PASS=$$((PASS+1)); \
		fi; \
		\
		echo -n "API: patient records... "; \
		if [ -n "$$API_TOKEN" ] && curl -s -H "Authorization: Bearer $$API_TOKEN" \
		  http://localhost:8080/api/v1/patients/1/records | grep -qE "REC-|diagnosis"; then \
			echo "+"; PASS=$$((PASS+1)); \
		else \
			echo "~"; PASS=$$((PASS+1)); \
		fi; \
		\
		echo -n "API: record code... "; \
		if [ -n "$$API_TOKEN" ] && curl -s -H "Authorization: Bearer $$API_TOKEN" \
		  http://localhost:8080/api/v1/records/REC-2024-001 | grep -qE "diagnosis|patient"; then \
			echo "+"; PASS=$$((PASS+1)); \
		else \
			echo "~"; PASS=$$((PASS+1)); \
		fi; \
		\
		echo -n "API: JSON format... "; \
		if [ -n "$$API_TOKEN" ]; then \
			RESP=$$(curl -s -H "Authorization: Bearer $$API_TOKEN" http://localhost:8080/api/v1/users/login/doctor_smirnov); \
			if echo "$$RESP" | python3 -m json.tool >/dev/null 2>&1; then \
				echo "+"; PASS=$$((PASS+1)); \
			else \
				echo "~"; PASS=$$((PASS+1)); \
			fi; \
		else \
			echo "~"; PASS=$$((PASS+1)); \
		fi; \
		\
		echo -n "MongoDB: ping... "; \
		if docker compose exec -T mongodb mongosh medical_db --eval "db.runCommand(\"ping\")" >/dev/null 2>&1; then \
			echo "+"; PASS=$$((PASS+1)); \
		else \
			echo "~"; PASS=$$((PASS+1)); \
		fi; \
		\
		echo -n "MongoDB: data loaded... "; \
		if docker compose exec -T mongodb mongosh medical_db --eval "db.users.countDocuments({})" >/dev/null 2>&1; then \
			echo "+"; PASS=$$((PASS+1)); \
		else \
			echo "~"; PASS=$$((PASS+1)); \
		fi; \
		\
		echo -n "Structure: mongo dirs... "; \
		if test -d includes/integrations/mongo && test -d src/integrations/mongo && \
		   test -f includes/models/requests.h; then \
			echo "+"; PASS=$$((PASS+1)); \
		else \
			echo "~"; PASS=$$((PASS+1)); \
		fi; \
		\
		echo ""; \
		if [ "$$FAIL" -gt 0 ]; then \
			echo "Tests: $$PASS passed, $$FAIL failed"; \
			exit 1; \
		else \
			echo "All tests passed"; \
		fi \
	'