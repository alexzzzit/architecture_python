#pragma once
#include "models/patient.h"
#include "models/medical_record.h"
#include <optional>
#include <vector>
#include <string>

namespace integrations::postgres {

class PatientRepository {
public:
    static std::optional<models::Patient> create(const models::PatientCreateRequest& req, const std::string& connStr);
    static std::optional<models::Patient> findById(const std::string& id, const std::string& connStr);
    static std::vector<models::Patient> searchByFullName(const std::string& firstName, const std::string& lastName, const std::string& middleName, const std::string& connStr);
    static std::optional<models::MedicalRecord> addRecord(const models::RecordCreateRequest& req, const std::string& connStr);
    static std::vector<models::MedicalRecord> getPatientRecords(const std::string& patientId, const std::string& connStr);
    static std::optional<models::MedicalRecord> getRecordByCode(const std::string& code, const std::string& connStr);
};

} // namespace integrations::postgres