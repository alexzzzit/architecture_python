#pragma once

#include "event.h"
#include <Poco/JSON/Object.h>
#include <Poco/JSON/Array.h>
#include <vector>

namespace MedicalSystem { namespace Events {

class MedicalRecordCreated : public Event {
public:
    MedicalRecordCreated(const std::string& recordId,
                        const std::string& patientId,
                        const std::string& doctorId,
                        const std::string& diagnosis,
                        const std::string& treatment,
                        const std::string& status,
                        Poco::Timestamp visitDate)
        : Event("MedicalRecordCreated", recordId, "MedicalRecord")
        , recordId_(recordId)
        , patientId_(patientId)
        , doctorId_(doctorId)
        , diagnosis_(diagnosis)
        , treatment_(treatment)
        , status_(status)
        , visitDate_(visitDate)
    {}

    Poco::JSON::Object::Ptr payload() const override {
        Poco::JSON::Object::Ptr payload = new Poco::JSON::Object;
        payload->set("record_id", recordId_);
        payload->set("patient_id", patientId_);
        payload->set("doctor_id", doctorId_);
        payload->set("diagnosis", diagnosis_);
        payload->set("treatment", treatment_);
        payload->set("status", status_);
        payload->set("visit_date", static_cast<double>(visitDate_.epochMicroseconds()) / 1000);
        return payload;
    }

    std::string recordId() const { return recordId_; }
    std::string patientId() const { return patientId_; }
    std::string diagnosis() const { return diagnosis_; }

private:
    std::string recordId_, patientId_, doctorId_, diagnosis_, treatment_, status_;
    Poco::Timestamp visitDate_;
};

class MedicalRecordUpdated : public Event {
public:
    MedicalRecordUpdated(const std::string& recordId,
                        const std::vector<std::string>& updatedFields,
                        const std::string& diagnosis,
                        const std::string& treatment)
        : Event("MedicalRecordUpdated", recordId, "MedicalRecord")
        , recordId_(recordId)
        , updatedFields_(updatedFields)
        , diagnosis_(diagnosis)
        , treatment_(treatment)
    {}

    Poco::JSON::Object::Ptr payload() const override {
        Poco::JSON::Object::Ptr payload = new Poco::JSON::Object;
        payload->set("record_id", recordId_);
        
        Poco::JSON::Array::Ptr fields = new Poco::JSON::Array;
        for (const auto& field : updatedFields_) {
            fields->add(field);
        }
        payload->set("updated_fields", fields);
        
        payload->set("diagnosis", diagnosis_);
        payload->set("treatment", treatment_);
        payload->set("updated_at", static_cast<double>(Poco::Timestamp().epochMicroseconds()) / 1000);
        return payload;
    }

private:
    std::string recordId_, diagnosis_, treatment_;
    std::vector<std::string> updatedFields_;
};

class MedicalRecordStatusChanged : public Event {
public:
    MedicalRecordStatusChanged(const std::string& recordId,
                              const std::string& oldStatus,
                              const std::string& newStatus,
                              const std::string& changedBy)
        : Event("MedicalRecordStatusChanged", recordId, "MedicalRecord")
        , recordId_(recordId)
        , oldStatus_(oldStatus)
        , newStatus_(newStatus)
        , changedBy_(changedBy)
    {}

    Poco::JSON::Object::Ptr payload() const override {
        Poco::JSON::Object::Ptr payload = new Poco::JSON::Object;
        payload->set("record_id", recordId_);
        payload->set("old_status", oldStatus_);
        payload->set("new_status", newStatus_);
        payload->set("changed_by", changedBy_);
        payload->set("changed_at", static_cast<double>(Poco::Timestamp().epochMicroseconds()) / 1000);
        return payload;
    }

private:
    std::string recordId_, oldStatus_, newStatus_, changedBy_;
};

}}