-- 1. Создание нового пользователя (POST /users)
INSERT INTO users (login, first_name, last_name, middle_name, email, password_hash, role)
VALUES ($1, $2, $3, $4, $5, $6, $7::user_role)
RETURNING id, login, first_name, last_name, email, role, created_at;

-- 2. Поиск пользователя по логину (GET /users/login/{login})
SELECT id, login, first_name, last_name, middle_name, email, role, is_active, created_at
FROM users WHERE login = $1 AND is_active = TRUE;

-- 3. Поиск пользователя по маске имени и фамилии (GET /users/search?mask=...)
SELECT id, login, first_name, last_name, middle_name, email, role
FROM users
WHERE (first_name ILIKE $1 OR last_name ILIKE $1 OR middle_name ILIKE $1)
  AND is_active = TRUE
ORDER BY last_name, first_name LIMIT 50;

-- 4. Регистрация пациента (POST /patients)
WITH new_user AS (
    INSERT INTO users (login, first_name, last_name, middle_name, email, password_hash, role)
    VALUES ($1, $2, $3, $4, $5, $6, 'patient'::user_role) RETURNING id
)
INSERT INTO patients (user_id, birth_date, phone, policy_number, snils, address)
SELECT id, $7, $8, $9, $10, $11 FROM new_user
RETURNING id AS patient_id, user_id, birth_date, policy_number, created_at;

-- 5. Поиск пациента по ФИО (GET /patients/search?fio=...)
SELECT p.id, u.first_name, u.last_name, u.middle_name, p.birth_date, p.phone, p.policy_number
FROM patients p JOIN users u ON p.user_id = u.id
WHERE (u.last_name ILIKE $1 OR u.first_name ILIKE $1 OR u.middle_name ILIKE $1)
   OR (u.last_name || ' ' || u.first_name || ' ' || COALESCE(u.middle_name, '')) ILIKE $1
ORDER BY u.last_name, u.first_name LIMIT 50;

-- 6. Создание медицинской записи (POST /records)
INSERT INTO medical_records (code, patient_id, doctor_id, diagnosis, description, treatment, notes, status, visit_date)
VALUES ($1, $2::uuid, $3::uuid, $4, $5, $6, $7, $8::record_status, $9)
RETURNING id, code, patient_id, doctor_id, diagnosis, treatment, status, visit_date, created_at;

-- 7. Получение истории записей пациента (GET /patients/{id}/records)
SELECT mr.id, mr.code, mr.diagnosis, mr.description, mr.treatment, mr.notes, mr.status, mr.visit_date, mr.created_at,
       u.first_name AS doctor_first_name, u.last_name AS doctor_last_name
FROM medical_records mr
JOIN users u ON mr.doctor_id = u.id
WHERE mr.patient_id = $1::uuid
ORDER BY mr.visit_date DESC, mr.created_at DESC
LIMIT $2 OFFSET $3;

-- 8. Получение записи по коду (GET /records/{code})
SELECT mr.id, mr.code, mr.diagnosis, mr.description, mr.treatment, mr.notes, mr.status, mr.visit_date, mr.created_at,
       pu.first_name AS patient_first_name, pu.last_name AS patient_last_name,
       du.first_name AS doctor_first_name, du.last_name AS doctor_last_name
FROM medical_records mr
JOIN patients p ON mr.patient_id = p.id
JOIN users pu ON p.user_id = pu.id
JOIN users du ON mr.doctor_id = du.id
WHERE mr.code = $1;