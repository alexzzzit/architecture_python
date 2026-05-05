#pragma once
#include "models/patient.h"
#include "models/medical_record.h"
#include <optional>
#include <vector>
#include <string>

namespace services {

class PatientService {
public:
    static PatientService& getInstance();
    
    std::optional<models::Patient> create(const models::PatientCreateRequest& req);
    std::optional<models::Patient> findById(const std::string& id);
    std::vector<models::Patient> searchByFullName(const std::string& firstName, const std::string& lastName, const std::string& middleName);
    std::optional<models::MedicalRecord> addRecord(const models::RecordCreateRequest& req);
    std::vector<models::MedicalRecord> getPatientRecords(const std::string& patientId);
    void clear();
    size_t patientCount() const;
    
private:
    PatientService() = default;
    static PatientService* instance_;
};

} // namespace services