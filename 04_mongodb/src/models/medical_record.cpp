#include "models/medical_record.h"
#include <Poco/DateTimeParser.h>
#include <Poco/DateTimeFormat.h>
#include <Poco/DateTimeFormatter.h>

namespace models {

Poco::MongoDB::Document Medication::toBson() const {
    Poco::MongoDB::Document doc;
    doc << "name" << name << "dosage" << dosage << "frequency" << frequency;
    return doc;
}

Medication Medication::fromBson(const Poco::MongoDB::Document& doc) {
    Medication med;
    med.name = doc.get<std::string>("name");
    med.dosage = doc.get<std::string>("dosage");
    med.frequency = doc.get<std::string>("frequency");
    return med;
}

Poco::MongoDB::Document TreatmentPlan::toBson() const {
    Poco::MongoDB::Document doc;
    auto& meds = doc.addNewArray("medications");
    for (const auto& m : medications) {
        meds.addNewDocument() << "name" << m.name << "dosage" << m.dosage << "frequency" << m.frequency;
    }
    auto& procs = doc.addNewArray("procedures");
    for (const auto& p : procedures) {
        procs.add(p);
    }
    doc << "recommendations" << recommendations;
    return doc;
}

TreatmentPlan TreatmentPlan::fromBson(const Poco::MongoDB::Document& doc) {
    TreatmentPlan plan;
    const auto& medsArr = doc.getArray("medications");
    for (size_t i = 0; i < medsArr.size(); ++i) {
        plan.medications.push_back(Medication::fromBson(medsArr.getObject(static_cast<int>(i))));
    }
    const auto& procsArr = doc.getArray("procedures");
    for (size_t i = 0; i < procsArr.size(); ++i) {
        plan.procedures.push_back(procsArr.get<std::string>(static_cast<int>(i)));
    }
    plan.recommendations = doc.get<std::string>("recommendations");
    return plan;
}

Poco::MongoDB::Document Attachment::toBson() const {
    Poco::MongoDB::Document doc;
    doc << "filename" << filename << "url" << url << "uploaded_at" << Poco::DateTimeFormatter::format(uploadedAt, Poco::DateTimeFormat::ISO8601_FORMAT);
    return doc;
}

Attachment Attachment::fromBson(const Poco::MongoDB::Document& doc) {
    Attachment att;
    att.filename = doc.get<std::string>("filename");
    att.url = doc.get<std::string>("url");
    att.uploadedAt = Poco::DateTimeParser::parse(Poco::DateTimeFormat::ISO8601_FORMAT, doc.get<std::string>("uploaded_at"));
    return att;
}

Poco::MongoDB::Document MedicalRecord::toBson() const {
    Poco::MongoDB::Document doc;
    doc << "id" << id
        << "code" << code
        << "patient_id" << patientId
        << "doctor_id" << doctorId
        << "diagnosis" << diagnosis
        << "symptoms" << Poco::MongoDB::Document::Array(symptoms.begin(), symptoms.end())
        << "treatment_plan" << treatmentPlan.toBson()
        << "status" << statusToString(status)
        << "visit_date" << Poco::DateTimeFormatter::format(visitDate, Poco::DateTimeFormat::ISO8601_FORMAT)
        << "created_at" << Poco::DateTimeFormatter::format(createdAt, Poco::DateTimeFormat::ISO8601_FORMAT)
        << "updated_at" << Poco::DateTimeFormatter::format(updatedAt, Poco::DateTimeFormat::ISO8601_FORMAT);

    auto& atts = doc.addNewArray("attachments");
    for (const auto& a : attachments) {
        atts.addNewDocument() << "filename" << a.filename << "url" << a.url << "uploaded_at" << Poco::DateTimeFormatter::format(a.uploadedAt, Poco::DateTimeFormat::ISO8601_FORMAT);
    }
    return doc;
}

MedicalRecord MedicalRecord::fromBson(const Poco::MongoDB::Document& doc) {
    MedicalRecord record;
    record.id = doc.get<long long>("id");
    record.code = doc.get<std::string>("code");
    record.patientId = doc.get<long long>("patient_id");
    record.doctorId = doc.get<long long>("doctor_id");
    record.diagnosis = doc.get<std::string>("diagnosis");

    const auto& symptomsArr = doc.getArray("symptoms");
    for (size_t i = 0; i < symptomsArr.size(); ++i) {
        record.symptoms.push_back(symptomsArr.get<std::string>(static_cast<int>(i)));
    }

    record.treatmentPlan = TreatmentPlan::fromBson(doc.getObject("treatment_plan"));
    record.status = statusFromString(doc.get<std::string>("status"));
    record.visitDate = Poco::DateTimeParser::parse(Poco::DateTimeFormat::ISO8601_FORMAT, doc.get<std::string>("visit_date"));
    record.createdAt = Poco::DateTimeParser::parse(Poco::DateTimeFormat::ISO8601_FORMAT, doc.get<std::string>("created_at"));
    record.updatedAt = Poco::DateTimeParser::parse(Poco::DateTimeFormat::ISO8601_FORMAT, doc.get<std::string>("updated_at"));

    const auto& attsArr = doc.getArray("attachments");
    for (size_t i = 0; i < attsArr.size(); ++i) {
        record.attachments.push_back(Attachment::fromBson(attsArr.getObject(static_cast<int>(i))));
    }
    return record;
}

MedicalRecord::Status MedicalRecord::statusFromString(const std::string& status) {
    if (status == "draft") return Status::DRAFT;
    if (status == "confirmed") return Status::CONFIRMED;
    if (status == "archived") return Status::ARCHIVED;
    return Status::DRAFT;
}

std::string MedicalRecord::statusToString(Status status) {
    switch (status) {
        case Status::DRAFT: return "draft";
        case Status::CONFIRMED: return "confirmed";
        case Status::ARCHIVED: return "archived";
        default: return "draft";
    }
}

} // namespace models
