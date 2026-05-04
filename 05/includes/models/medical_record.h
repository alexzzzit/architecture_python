#pragma once

#include <string>
#include <vector>
#include <Poco/Timestamp.h>
#include <Poco/MongoDB/Document.h>

namespace models {

struct Medication {
    std::string name;
    std::string dosage;
    std::string frequency;

    Poco::MongoDB::Document toBson() const;
    static Medication fromBson(const Poco::MongoDB::Document& doc);
};

struct TreatmentPlan {
    std::vector<Medication> medications;
    std::vector<std::string> procedures;
    std::string recommendations;

    Poco::MongoDB::Document toBson() const;
    static TreatmentPlan fromBson(const Poco::MongoDB::Document& doc);
};

struct Attachment {
    std::string filename;
    std::string url;
    Poco::Timestamp uploadedAt;

    Poco::MongoDB::Document toBson() const;
    static Attachment fromBson(const Poco::MongoDB::Document& doc);
};

class MedicalRecord {
public:
    enum class Status {
        DRAFT,
        CONFIRMED,
        ARCHIVED
    };

    MedicalRecord() = default;

    long long id = 0;
    std::string code;
    long long patientId = 0;
    long long doctorId = 0;
    std::string diagnosis;
    std::vector<std::string> symptoms;
    TreatmentPlan treatmentPlan;
    std::vector<Attachment> attachments;
    Status status = Status::DRAFT;
    Poco::Timestamp visitDate;
    Poco::Timestamp createdAt;
    Poco::Timestamp updatedAt;

    Poco::MongoDB::Document toBson() const;
    static MedicalRecord fromBson(const Poco::MongoDB::Document& doc);

private:
    static Status statusFromString(const std::string& status);
    static std::string statusToString(Status status);
};

} // namespace models