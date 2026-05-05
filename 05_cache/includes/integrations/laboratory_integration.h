#pragma once
#include <string>
#include <vector>
#include <optional>

namespace integrations {

struct LabResult {
    std::string testCode;
    std::string testName;
    std::string result;
    std::string unit;
    std::string referenceRange;
    int64_t performedAt;
};

class LaboratoryIntegration {
public:
    static LaboratoryIntegration& getInstance();
    
    std::vector<LabResult> getPatientResults(const std::string& patientId);
    std::optional<LabResult> getResultByCode(const std::string& resultCode);
    bool isMockMode() const;
    void setMockMode(bool enabled);

private:
    LaboratoryIntegration();
    LaboratoryIntegration(const LaboratoryIntegration&) = delete;
    LaboratoryIntegration& operator=(const LaboratoryIntegration&) = delete;
    
    bool _mockMode;
    std::string _baseUrl;
};

}