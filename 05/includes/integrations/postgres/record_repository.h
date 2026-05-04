#pragma once
#include "models/medical_record.h"
#include <optional>
#include <vector>
#include <string>

namespace integrations::postgres {

class RecordRepository {
public:
    static std::optional<models::MedicalRecord> create(const models::RecordCreateRequest& req, const std::string& connStr);
    static std::optional<models::MedicalRecord> findByCode(const std::string& code, const std::string& connStr);
    static std::vector<models::MedicalRecord> findByPatientId(const std::string& patientId, const std::string& connStr);
    static size_t count(const std::string& connStr);
};

} // namespace integrations::postgres