CREATE EXTENSION IF NOT EXISTS "uuid-ossp";
CREATE EXTENSION IF NOT EXISTS "pg_trgm";

-- Перечисления
CREATE TYPE user_role AS ENUM ('patient', 'doctor', 'admin', 'lab_technician');
CREATE TYPE record_status AS ENUM ('draft', 'confirmed', 'archived');

-- 1. Пользователи
CREATE TABLE users (
    id UUID PRIMARY KEY DEFAULT uuid_generate_v4(),
    login VARCHAR(100) UNIQUE NOT NULL,
    first_name VARCHAR(100) NOT NULL,
    last_name VARCHAR(100) NOT NULL,
    middle_name VARCHAR(100),
    email VARCHAR(255) UNIQUE NOT NULL,
    password_hash VARCHAR(255) NOT NULL,
    role user_role NOT NULL,
    is_active BOOLEAN DEFAULT TRUE,
    created_at TIMESTAMPTZ DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMPTZ DEFAULT CURRENT_TIMESTAMP,
    CONSTRAINT chk_login_len CHECK (char_length(login) >= 3)
);

-- 2. Пациенты
CREATE TABLE patients (
    id UUID PRIMARY KEY DEFAULT uuid_generate_v4(),
    user_id UUID UNIQUE NOT NULL REFERENCES users(id) ON DELETE CASCADE,
    birth_date DATE NOT NULL,
    phone VARCHAR(20),
    policy_number VARCHAR(50) UNIQUE NOT NULL,
    snils VARCHAR(20) UNIQUE,
    address TEXT,
    created_at TIMESTAMPTZ DEFAULT CURRENT_TIMESTAMP,
    CONSTRAINT chk_birth_date CHECK (birth_date <= CURRENT_DATE)
);

-- 3. Медицинские записи
CREATE TABLE medical_records (
    id UUID PRIMARY KEY DEFAULT uuid_generate_v4(),
    code VARCHAR(50) UNIQUE NOT NULL,
    patient_id UUID NOT NULL REFERENCES patients(id) ON DELETE CASCADE,
    doctor_id UUID NOT NULL REFERENCES users(id) ON DELETE RESTRICT,
    diagnosis TEXT NOT NULL,
    description TEXT,
    treatment TEXT,
    notes TEXT,
    status record_status DEFAULT 'draft',
    visit_date TIMESTAMPTZ DEFAULT CURRENT_TIMESTAMP,
    created_at TIMESTAMPTZ DEFAULT CURRENT_TIMESTAMP,
    CONSTRAINT chk_code_format CHECK (code ~ '^[A-Z0-9\-]{8,50}$')
);

-- Быстрый поиск по логину (авторизация)
CREATE INDEX idx_users_login ON users(login);
-- Нечёткий поиск по ФИО (маска)
CREATE INDEX idx_users_name_trgm ON users USING gin((first_name || ' ' || last_name || ' ' || COALESCE(middle_name, '')) gin_trgm_ops);
-- Связь patients ↔ users (для JOIN)
CREATE INDEX idx_patients_user_id ON patients(user_id);
-- Поиск по номеру полиса ОМС
CREATE INDEX idx_patients_policy ON patients(policy_number);
-- Поиск записи по уникальному коду
CREATE INDEX idx_records_code ON medical_records(code);
-- История записей пациента + сортировка по дате
CREATE INDEX idx_records_patient_visit ON medical_records(patient_id, visit_date DESC);
-- Полнотекстовый поиск по диагнозу
CREATE INDEX idx_records_diagnosis_trgm ON medical_records USING gin(diagnosis gin_trgm_ops);


CREATE OR REPLACE FUNCTION update_updated_at_column()
RETURNS TRIGGER AS $$
BEGIN
    NEW.updated_at = CURRENT_TIMESTAMP;
    RETURN NEW;
END;
$$ LANGUAGE plpgsql;

CREATE TRIGGER trg_users_upd BEFORE UPDATE ON users FOR EACH ROW EXECUTE FUNCTION update_updated_at_column();
CREATE TRIGGER trg_patients_upd BEFORE UPDATE ON patients FOR EACH ROW EXECUTE FUNCTION update_updated_at_column();
CREATE TRIGGER trg_records_upd BEFORE UPDATE ON medical_records FOR EACH ROW EXECUTE FUNCTION update_updated_at_column();