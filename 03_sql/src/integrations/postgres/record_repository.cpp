#include "integrations/postgres/record_repository.h"
#include "integrations/postgres/connection_pool.h"
#include <libpq-fe.h>
#include <iostream>

namespace integrations::postgres {

std::optional<models::MedicalRecord> RecordRepository::create(const models::RecordCreateRequest& req, const std::string& connStr) {
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

std::optional<models::MedicalRecord> RecordRepository::findByCode(const std::string& code, const std::string& connStr) {
    const char* query = "SELECT id, code, patient_id, doctor_id, diagnosis, description, treatment, notes, status, visit_date FROM medical_records WHERE code = $1";
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
    
    PQclear(res);
    return rec;
}

std::vector<models::MedicalRecord> RecordRepository::findByPatientId(const std::string& patientId, const std::string& connStr) {
    std::vector<models::MedicalRecord> results;
    const char* query = "SELECT id, code, diagnosis, treatment, status, visit_date FROM medical_records WHERE patient_id = $1::uuid ORDER BY visit_date DESC";
    const char* params[1] = { patientId.c_str() };
    
    PGresult* res = ConnectionPool::getInstance(connStr).execParams(query, 1, params);
    if (res && PQresultStatus(res) == PGRES_TUPLES_OK) {
        for (int i = 0; i < PQntuples(res); ++i) {
            models::MedicalRecord rec;
            rec.id = PQgetvalue(res, i, 0);
            rec.code = PQgetvalue(res, i, 1);
            rec.diagnosis = PQgetvalue(res, i, 2);
            rec.treatment = PQgetvalue(res, i, 3) ? PQgetvalue(res, i, 3) : "";
            rec.status = PQgetvalue(res, i, 4);
            rec.visitDate = std::time(nullptr);
            results.push_back(rec);
        }
    }
    if (res) PQclear(res);
    return results;
}

size_t RecordRepository::count(const std::string& connStr) {
    PGresult* res = ConnectionPool::getInstance(connStr).exec("SELECT COUNT(*) FROM medical_records");
    if (!res || PQresultStatus(res) != PGRES_TUPLES_OK) {
        if (res) PQclear(res);
        return 0;
    }
    size_t cnt = std::stoul(PQgetvalue(res, 0, 0));
    PQclear(res);
    return cnt;
}

} // namespace integrations::postgres