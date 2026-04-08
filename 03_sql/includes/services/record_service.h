#pragma once
#include "models/medical_record.h"
#include <optional>
#include <vector>
#include <string>

namespace services {

class RecordService {
public:
    static RecordService& getInstance();
    
    std::optional<models::MedicalRecord> create(const models::RecordCreateRequest& req);
    std::optional<models::MedicalRecord> findByCode(const std::string& code);
    std::vector<models::MedicalRecord> findByPatientId(const std::string& patientId);
    size_t count() const;
    void clear();
    
private:
    RecordService() = default;
    static RecordService* instance_;
};

} // namespace services