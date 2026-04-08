#pragma once
#include <string>
#include <ctime>

namespace models {

struct MedicalRecord {
    std::string id;
    std::string code;
    std::string patientId;
    std::string doctorId;
    std::string diagnosis;
    std::string description;
    std::string treatment;
    std::string notes;
    std::string status;
    time_t visitDate = 0;
    time_t createdAt = 0;
};

struct RecordCreateRequest {
    std::string code;
    std::string patientId;
    std::string doctorId;
    std::string diagnosis;
    std::string description;
    std::string treatment;
    std::string notes;
    std::string status = "confirmed";
};

} // namespace models