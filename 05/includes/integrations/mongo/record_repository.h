#pragma once

#include <Poco/MongoDB/Database.h>
#include <Poco/MongoDB/Document.h>
#include <optional>
#include <vector>
#include "models/medical_record.h"

namespace db {

class RecordRepository {
public:
    explicit RecordRepository(Poco::MongoDB::Database& database);

    bool create(const models::MedicalRecord& record);
    std::optional<models::MedicalRecord> findById(long long id);
    std::optional<models::MedicalRecord> findByCode(const std::string& code);
    std::vector<models::MedicalRecord> findByPatientId(long long patientId, const std::string& status = "");
    std::vector<models::MedicalRecord> findByDoctorId(long long doctorId);
    std::vector<models::MedicalRecord> findByDateRange(long long patientId, const std::string& from, const std::string& to);
    std::vector<models::MedicalRecord> findByDiagnosisPattern(const std::string& pattern);
    std::vector<models::MedicalRecord> findAll();
    bool update(long long id, const models::MedicalRecord& record);
    bool updateStatus(long long id, const std::string& status);
    bool remove(long long id);

private:
    Poco::MongoDB::Document toDocument(const models::MedicalRecord& record) const;
    models::MedicalRecord fromDocument(const Poco::MongoDB::Document& doc) const;

    Poco::MongoDB::Database& database_;
};

} // namespace db