#include "integrations/mongo/record_repository.h"
#include <Poco/MongoDB/Document.h>
#include <Poco/MongoDB/Cursor.h>
#include <Poco/DateTimeFormatter.h>
#include <Poco/DateTimeFormat.h>
#include <Poco/DateTimeParser.h>

namespace db {

using namespace Poco::MongoDB;

RecordRepository::RecordRepository(Database& database) : database_(database) {}

Document RecordRepository::toDocument(const models::MedicalRecord& record) const {
    Document doc;
    doc << "id" << record.id
        << "code" << record.code
        << "patient_id" << record.patientId
        << "doctor_id" << record.doctorId
        << "diagnosis" << record.diagnosis
        << "symptoms" << Document::Array(record.symptoms.begin(), record.symptoms.end())
        << "treatment_plan" << record.treatmentPlan.toBson()
        << "status" << models::MedicalRecord::statusToString(record.status)
        << "visit_date" << Poco::DateTimeFormatter::format(record.visitDate, Poco::DateTimeFormat::ISO8601_FORMAT)
        << "created_at" << Poco::DateTimeFormatter::format(record.createdAt, Poco::DateTimeFormat::ISO8601_FORMAT)
        << "updated_at" << Poco::DateTimeFormatter::format(record.updatedAt, Poco::DateTimeFormat::ISO8601_FORMAT);

    auto& attachments = doc.addNewArray("attachments");
    for (const auto& att : record.attachments) {
        attachments.addNewDocument() 
            << "filename" << att.filename 
            << "url" << att.url 
            << "uploaded_at" << Poco::DateTimeFormatter::format(att.uploadedAt, Poco::DateTimeFormat::ISO8601_FORMAT);
    }
    return doc;
}

models::MedicalRecord RecordRepository::fromDocument(const Document& doc) const {
    models::MedicalRecord record;
    record.id = doc.get<long long>("id");
    record.code = doc.get<std::string>("code");
    record.patientId = doc.get<long long>("patient_id");
    record.doctorId = doc.get<long long>("doctor_id");
    record.diagnosis = doc.get<std::string>("diagnosis");

    if (doc.has("symptoms")) {
        const auto& symptomsArr = doc.getArray("symptoms");
        for (size_t i = 0; i < symptomsArr.size(); ++i) {
            record.symptoms.push_back(symptomsArr.get<std::string>(static_cast<int>(i)));
        }
    }

    if (doc.has("treatment_plan")) {
        record.treatmentPlan = models::TreatmentPlan::fromBson(doc.getObject("treatment_plan"));
    }

    if (doc.has("status")) {
        record.status = models::MedicalRecord::statusFromString(doc.get<std::string>("status"));
    }
    
    if (doc.has("visit_date")) {
        record.visitDate = Poco::DateTimeParser::parse(Poco::DateTimeFormat::ISO8601_FORMAT, doc.get<std::string>("visit_date"));
    }
    if (doc.has("created_at")) {
        record.createdAt = Poco::DateTimeParser::parse(Poco::DateTimeFormat::ISO8601_FORMAT, doc.get<std::string>("created_at"));
    }
    if (doc.has("updated_at")) {
        record.updatedAt = Poco::DateTimeParser::parse(Poco::DateTimeFormat::ISO8601_FORMAT, doc.get<std::string>("updated_at"));
    }

    if (doc.has("attachments")) {
        const auto& attachmentsArr = doc.getArray("attachments");
        for (size_t i = 0; i < attachmentsArr.size(); ++i) {
            record.attachments.push_back(models::Attachment::fromBson(attachmentsArr.getObject(static_cast<int>(i))));
        }
    }
    return record;
}

bool RecordRepository::create(const models::MedicalRecord& record) {
    try {
        auto collection = database_.getCollection("medical_records");
        return collection.insert(toDocument(record)) > 0;
    } catch (...) {
        return false;
    }
}

std::optional<models::MedicalRecord> RecordRepository::findById(long long id) {
    try {
        auto collection = database_.getCollection("medical_records");
        Document selector;
        selector << "id" << id;
        Cursor cursor(collection, selector);
        auto response = cursor.next();
        if (response->documents().size() > 0) {
            return fromDocument(*response->documents()[0]);
        }
    } catch (...) {}
    return std::nullopt;
}

std::optional<models::MedicalRecord> RecordRepository::findByCode(const std::string& code) {
    try {
        auto collection = database_.getCollection("medical_records");
        Document selector;
        selector << "code" << code;
        Cursor cursor(collection, selector);
        auto response = cursor.next();
        if (response->documents().size() > 0) {
            return fromDocument(*response->documents()[0]);
        }
    } catch (...) {}
    return std::nullopt;
}

std::vector<models::MedicalRecord> RecordRepository::findByPatientId(long long patientId, const std::string& status) {
    std::vector<models::MedicalRecord> result;
    try {
        auto collection = database_.getCollection("medical_records");
        Document selector;
        selector << "patient_id" << patientId;
        if (!status.empty()) {
            selector << "status" << status;
        }
        Cursor cursor(collection, selector);
        cursor.sortBy("visit_date", -1);
        auto response = cursor.next();
        for (const auto& doc : response->documents()) {
            result.push_back(fromDocument(*doc));
        }
    } catch (...) {}
    return result;
}

std::vector<models::MedicalRecord> RecordRepository::findByDoctorId(long long doctorId) {
    std::vector<models::MedicalRecord> result;
    try {
        auto collection = database_.getCollection("medical_records");
        Document selector;
        selector << "doctor_id" << doctorId;
        Cursor cursor(collection, selector);
        cursor.sortBy("visit_date", -1);
        auto response = cursor.next();
        for (const auto& doc : response->documents()) {
            result.push_back(fromDocument(*doc));
        }
    } catch (...) {}
    return result;
}

std::vector<models::MedicalRecord> RecordRepository::findByDateRange(long long patientId, const std::string& from, const std::string& to) {
    std::vector<models::MedicalRecord> result;
    try {
        auto collection = database_.getCollection("medical_records");
        Document selector;
        Document dateRange;
        dateRange << "$gte" << from << "$lte" << to;
        selector << "patient_id" << patientId << "visit_date" << dateRange;
        Cursor cursor(collection, selector);
        cursor.sortBy("visit_date", -1);
        auto response = cursor.next();
        for (const auto& doc : response->documents()) {
            result.push_back(fromDocument(*doc));
        }
    } catch (...) {}
    return result;
}

std::vector<models::MedicalRecord> RecordRepository::findByDiagnosisPattern(const std::string& pattern) {
    std::vector<models::MedicalRecord> result;
    try {
        auto collection = database_.getCollection("medical_records");
        Document selector;
        Document regexClause;
        regexClause << "$regex" << pattern << "$options" << "i";
        selector << "diagnosis" << regexClause;
        Cursor cursor(collection, selector);
        auto response = cursor.next();
        for (const auto& doc : response->documents()) {
            result.push_back(fromDocument(*doc));
        }
    } catch (...) {}
    return result;
}

std::vector<models::MedicalRecord> RecordRepository::findAll() {
    std::vector<models::MedicalRecord> result;
    try {
        auto collection = database_.getCollection("medical_records");
        Document selector;
        Cursor cursor(collection, selector);
        auto response = cursor.next();
        for (const auto& doc : response->documents()) {
            result.push_back(fromDocument(*doc));
        }
    } catch (...) {}
    return result;
}

bool RecordRepository::update(long long id, const models::MedicalRecord& record) {
    try {
        auto collection = database_.getCollection("medical_records");
        Document selector;
        selector << "id" << id;
        Document updateDoc;
        updateDoc << "$set" << toDocument(record);
        return collection.update(selector, updateDoc) > 0;
    } catch (...) {
        return false;
    }
}

bool RecordRepository::updateStatus(long long id, const std::string& status) {
    try {
        auto collection = database_.getCollection("medical_records");
        Document selector;
        selector << "id" << id;
        Document updateDoc;
        updateDoc << "$set" << Document() 
                  << "status" << status 
                  << "updated_at" << Poco::DateTimeFormatter::format(Poco::Timestamp(), Poco::DateTimeFormat::ISO8601_FORMAT);
        return collection.update(selector, updateDoc) > 0;
    } catch (...) {
        return false;
    }
}

bool RecordRepository::remove(long long id) {
    try {
        auto collection = database_.getCollection("medical_records");
        Document selector;
        selector << "id" << id;
        return collection.remove(selector) > 0;
    } catch (...) {
        return false;
    }
}

} // namespace db