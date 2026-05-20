#include "integrations/postgres/patient_repository.h"
#include "integrations/postgres/connection_pool.h"
#include <libpq-fe.h>
#include <iostream>
#include <sstream>

namespace integrations::postgres {

std::optional<models::Patient> PatientRepository::create(const models::PatientCreateRequest& req, const std::string& connStr) {
    const char* query = R"(
        WITH new_user AS (
            INSERT INTO users (login, first_name, last_name, middle_name, email, password_hash, role)
            VALUES ($1, $2, $3, $4, $5, $6, 'patient'::user_role) RETURNING id
        )
        INSERT INTO patients (user_id, birth_date, phone, policy_number, snils, address)
        SELECT id, $7, $8, $9, $10, $11 FROM new_user
        RETURNING id, user_id, birth_date, policy_number, created_at
    )";
    
    std::string login = "patient_" + req.firstName + "_" + req.lastName;
    std::string email = req.firstName + "." + req.lastName + "@example.com";
    
    const char* params[11] = {
        login.c_str(), req.firstName.c_str(), req.lastName.c_str(),
        req.middleName.c_str(), email.c_str(), "$2b$12$placeholder_hash",
        req.birthDate.c_str(), req.phone.c_str(), req.insuranceNumber.c_str(),
        req.snils.c_str(), req.address.c_str()
    };
    
    PGresult* res = ConnectionPool::getInstance(connStr).execParams(query, 11, params);
    if (!res || PQresultStatus(res) != PGRES_TUPLES_OK) {
        if (res) PQclear(res);
        return std::nullopt;
    }
    
    models::Patient p;
    p.id = PQgetvalue(res, 0, 0);
    p.userId = PQgetvalue(res, 0, 1);
    p.birthDate = PQgetvalue(res, 0, 2);
    p.insuranceNumber = PQgetvalue(res, 0, 3);
    p.createdAt = std::time(nullptr);
    
    PQclear(res);
    return p;
}

std::optional<models::Patient> PatientRepository::findById(const std::string& id, const std::string& connStr) {
    const char* query = R"(
        SELECT p.id, p.user_id, u.first_name, u.last_name, u.middle_name, p.birth_date, p.phone, p.policy_number, p.snils, p.address, p.created_at
        FROM patients p JOIN users u ON p.user_id = u.id WHERE p.id = $1
    )";
    const char* params[1] = { id.c_str() };
    
    PGresult* res = ConnectionPool::getInstance(connStr).execParams(query, 1, params);
    if (!res || PQresultStatus(res) != PGRES_TUPLES_OK || PQntuples(res) == 0) {
        if (res) PQclear(res);
        return std::nullopt;
    }
    
    models::Patient p;
    p.id = PQgetvalue(res, 0, 0);
    p.userId = PQgetvalue(res, 0, 1);
    p.firstName = PQgetvalue(res, 0, 2);
    p.lastName = PQgetvalue(res, 0, 3);
    p.middleName = PQgetvalue(res, 0, 4) ? PQgetvalue(res, 0, 4) : "";
    p.birthDate = PQgetvalue(res, 0, 5);
    p.phone = PQgetvalue(res, 0, 6);
    p.insuranceNumber = PQgetvalue(res, 0, 7);
    p.snils = PQgetvalue(res, 0, 8) ? PQgetvalue(res, 0, 8) : "";
    p.address = PQgetvalue(res, 0, 9) ? PQgetvalue(res, 0, 9) : "";
    p.createdAt = std::time(nullptr);
    
    PQclear(res);
    return p;
}

std::vector<models::Patient> PatientRepository::searchByFullName(const std::string& firstName, const std::string& lastName, const std::string& middleName, const std::string& connStr) {
    std::vector<models::Patient> results;
    std::string query = R"(
        SELECT p.id, p.user_id, u.first_name, u.last_name, u.middle_name, p.birth_date, p.policy_number
        FROM patients p JOIN users u ON p.user_id = u.id WHERE true
    )";
    
    std::vector<const char*> params;
    int paramIdx = 1;
    
    if (!firstName.empty()) {
        query += " AND u.first_name ILIKE $" + std::to_string(paramIdx++);
        params.push_back(("%" + firstName + "%").c_str());
    }
    if (!lastName.empty()) {
        query += " AND u.last_name ILIKE $" + std::to_string(paramIdx++);
        params.push_back(("%" + lastName + "%").c_str());
    }
    if (!middleName.empty()) {
        query += " AND u.middle_name ILIKE $" + std::to_string(paramIdx++);
        params.push_back(("%" + middleName + "%").c_str());
    }
    
    query += " ORDER BY u.last_name, u.first_name LIMIT 50";
    
    PGresult* res = ConnectionPool::getInstance(connStr).execParams(query.c_str(), params.size(), params.data());
    
    if (res && PQresultStatus(res) == PGRES_TUPLES_OK) {
        for (int i = 0; i < PQntuples(res); ++i) {
            models::Patient p;
            p.id = PQgetvalue(res, i, 0);
            p.userId = PQgetvalue(res, i, 1);
            p.firstName = PQgetvalue(res, i, 2);
            p.lastName = PQgetvalue(res, i, 3);
            p.middleName = PQgetvalue(res, i, 4) ? PQgetvalue(res, i, 4) : "";
            p.birthDate = PQgetvalue(res, i, 5);
            p.insuranceNumber = PQgetvalue(res, i, 6);
            results.push_back(p);
        }
    }
    if (res) PQclear(res);
    return results;
}

std::optional<models::MedicalRecord> PatientRepository::addRecord(const models::RecordCreateRequest& req, const std::string& connStr) {
    std::string code = "REC-" + std::to_string(std::time(nullptr)) + "-" + req.patientId.substr(0, 8);
    
    const char* query = R"(
        INSERT INTO medical_records (code, patient_id, doctor_id, diagnosis, description, treatment, notes, status, visit_date)
        VALUES ($1, $2::uuid, $3::uuid, $4, $5, $6, $7, $8::record_status, NOW())
        RETURNING id, code, patient_id, doctor_id, diagnosis, treatment, status, visit_date, created_at
    )";
    
    const char* params[8] = {
        code.c_str(), req.patientId.c_str(), req.doctorId.c_str(),
        req.diagnosis.c_str(), req.description.c_str(), req.treatment.c_str(),
        req.notes.c_str(), "confirmed"
    };
    
    PGresult* res = ConnectionPool::getInstance(connStr).execParams(query, 8, params);
    if (!res || PQresultStatus(res) != PGRES_TUPLES_OK) {
        if (res) PQclear(res);
        return std::nullopt;
    }
    
    models::MedicalRecord rec;
    rec.id = PQgetvalue(res, 0, 0);
    rec.code = PQgetvalue(res, 0, 1);
    rec.patientId = PQgetvalue(res, 0, 2);
    rec.doctorId = PQgetvalue(res, 0, 3);
    rec.diagnosis = PQgetvalue(res, 0, 4);
    rec.treatment = PQgetvalue(res, 0, 5);
    rec.status = PQgetvalue(res, 0, 6);
    rec.visitDate = std::time(nullptr);
    rec.createdAt = std::time(nullptr);
    
    PQclear(res);
    return rec;
}

std::vector<models::MedicalRecord> PatientRepository::getPatientRecords(const std::string& patientId, const std::string& connStr) {
    std::vector<models::MedicalRecord> results;
    const char* query = R"(
        SELECT id, code, diagnosis, description, treatment, notes, status, visit_date, created_at
        FROM medical_records WHERE patient_id = $1::uuid ORDER BY visit_date DESC
    )";
    const char* params[1] = { patientId.c_str() };
    
    PGresult* res = ConnectionPool::getInstance(connStr).execParams(query, 1, params);
    if (res && PQresultStatus(res) == PGRES_TUPLES_OK) {
        for (int i = 0; i < PQntuples(res); ++i) {
            models::MedicalRecord rec;
            rec.id = PQgetvalue(res, i, 0);
            rec.code = PQgetvalue(res, i, 1);
            rec.diagnosis = PQgetvalue(res, i, 2);
            rec.description = PQgetvalue(res, i, 3) ? PQgetvalue(res, i, 3) : "";
            rec.treatment = PQgetvalue(res, i, 4) ? PQgetvalue(res, i, 4) : "";
            rec.notes = PQgetvalue(res, i, 5) ? PQgetvalue(res, i, 5) : "";
            rec.status = PQgetvalue(res, i, 6);
            rec.visitDate = std::time(nullptr);
            rec.createdAt = std::time(nullptr);
            results.push_back(rec);
        }
    }
    if (res) PQclear(res);
    return results;
}

std::optional<models::MedicalRecord> PatientRepository::getRecordByCode(const std::string& code, const std::string& connStr) {
    const char* query = R"(
        SELECT id, code, patient_id, doctor_id, diagnosis, description, treatment, notes, status, visit_date, created_at
        FROM medical_records WHERE code = $1
    )";
    const char* params[1] = { code.c_str() };
    
    PGresult* res = ConnectionPool::getInstance(connStr).execParams(query, 1, params);
    if (!res || PQresultStatus(res) != PGRES_TUPLES_OK || PQntuples(res) == 0) {
        if (res) PQclear(res);
        return std::nullopt;
    }
    
    models::MedicalRecord rec;
    rec.id = PQgetvalue(res, 0, 0);
    rec.code = PQgetvalue(res, 0, 1);
    rec.patientId = PQgetvalue(res, 0, 2);
    rec.doctorId = PQgetvalue(res, 0, 3);
    rec.diagnosis = PQgetvalue(res, 0, 4);
    rec.description = PQgetvalue(res, 0, 5) ? PQgetvalue(res, 0, 5) : "";
    rec.treatment = PQgetvalue(res, 0, 6) ? PQgetvalue(res, 0, 6) : "";
    rec.notes = PQgetvalue(res, 0, 7) ? PQgetvalue(res, 0, 7) : "";
    rec.status = PQgetvalue(res, 0, 8);
    rec.visitDate = std::time(nullptr);
    rec.createdAt = std::time(nullptr);
    
    PQclear(res);
    return rec;
}

} // namespace integrations::postgres