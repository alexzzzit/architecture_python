#include "integrations/mongo/patient_repository.h"
#include <Poco/MongoDB/Document.h>
#include <Poco/MongoDB/Cursor.h>
#include <Poco/DateTimeFormatter.h>
#include <Poco/DateTimeFormat.h>
#include <Poco/DateTimeParser.h>

namespace db {

using namespace Poco::MongoDB;

PatientRepository::PatientRepository(Database& database) : database_(database) {}

Document PatientRepository::toDocument(const models::Patient& patient) const {
    Document doc;
    doc << "id" << patient.id
        << "user_id" << patient.userId
        << "policy_number" << patient.policyNumber
        << "snils" << patient.snils
        << "first_name" << patient.firstName
        << "last_name" << patient.lastName
        << "middle_name" << patient.middleName
        << "birth_date" << Poco::DateTimeFormatter::format(patient.birthDate, Poco::DateTimeFormat::ISO8601_FORMAT)
        << "phone" << patient.phone
        << "address" << patient.address.toBson()
        << "emergency_contact" << patient.emergencyContact.toBson()
        << "created_at" << Poco::DateTimeFormatter::format(patient.createdAt, Poco::DateTimeFormat::ISO8601_FORMAT)
        << "updated_at" << Poco::DateTimeFormatter::format(patient.updatedAt, Poco::DateTimeFormat::ISO8601_FORMAT);
    return doc;
}

models::Patient PatientRepository::fromDocument(const Document& doc) const {
    models::Patient patient;
    patient.id = doc.get<long long>("id");
    patient.userId = doc.get<long long>("user_id");
    patient.policyNumber = doc.get<std::string>("policy_number");
    patient.snils = doc.get<std::string>("snils");
    patient.firstName = doc.get<std::string>("first_name");
    patient.lastName = doc.get<std::string>("last_name");
    patient.middleName = doc.get<std::string>("middle_name");
    
    if (doc.has("birth_date")) {
        patient.birthDate = Poco::DateTimeParser::parse(Poco::DateTimeFormat::ISO8601_FORMAT, doc.get<std::string>("birth_date"));
    }
    
    patient.phone = doc.get<std::string>("phone");
    
    if (doc.has("address")) {
        patient.address = models::Address::fromBson(doc.getObject("address"));
    }
    if (doc.has("emergency_contact")) {
        patient.emergencyContact = models::EmergencyContact::fromBson(doc.getObject("emergency_contact"));
    }
    
    if (doc.has("created_at")) {
        patient.createdAt = Poco::DateTimeParser::parse(Poco::DateTimeFormat::ISO8601_FORMAT, doc.get<std::string>("created_at"));
    }
    if (doc.has("updated_at")) {
        patient.updatedAt = Poco::DateTimeParser::parse(Poco::DateTimeFormat::ISO8601_FORMAT, doc.get<std::string>("updated_at"));
    }
    return patient;
}

bool PatientRepository::create(const models::Patient& patient) {
    try {
        auto collection = database_.getCollection("patients");
        return collection.insert(toDocument(patient)) > 0;
    } catch (...) {
        return false;
    }
}

std::optional<models::Patient> PatientRepository::findById(long long id) {
    try {
        auto collection = database_.getCollection("patients");
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

std::optional<models::Patient> PatientRepository::findByUserId(long long userId) {
    try {
        auto collection = database_.getCollection("patients");
        Document selector;
        selector << "user_id" << userId;
        Cursor cursor(collection, selector);
        auto response = cursor.next();
        if (response->documents().size() > 0) {
            return fromDocument(*response->documents()[0]);
        }
    } catch (...) {}
    return std::nullopt;
}

std::optional<models::Patient> PatientRepository::findByPolicyNumber(const std::string& policyNumber) {
    try {
        auto collection = database_.getCollection("patients");
        Document selector;
        selector << "policy_number" << policyNumber;
        Cursor cursor(collection, selector);
        auto response = cursor.next();
        if (response->documents().size() > 0) {
            return fromDocument(*response->documents()[0]);
        }
    } catch (...) {}
    return std::nullopt;
}

std::optional<models::Patient> PatientRepository::findBySnils(const std::string& snils) {
    try {
        auto collection = database_.getCollection("patients");
        Document selector;
        selector << "snils" << snils;
        Cursor cursor(collection, selector);
        auto response = cursor.next();
        if (response->documents().size() > 0) {
            return fromDocument(*response->documents()[0]);
        }
    } catch (...) {}
    return std::nullopt;
}

std::vector<models::Patient> PatientRepository::findByNamePattern(const std::string& firstName, const std::string& lastName) {
    std::vector<models::Patient> result;
    try {
        auto collection = database_.getCollection("patients");
        Document orClause;
        orClause << "$or" << Document::Array{
            Document() << "last_name" << Document() << "$regex" << lastName << "$options" << "i",
            Document() << "first_name" << Document() << "$regex" << firstName << "$options" << "i"
        };
        Cursor cursor(collection, orClause);
        auto response = cursor.next();
        for (const auto& doc : response->documents()) {
            result.push_back(fromDocument(*doc));
        }
    } catch (...) {}
    return result;
}

std::vector<models::Patient> PatientRepository::findAll() {
    std::vector<models::Patient> result;
    try {
        auto collection = database_.getCollection("patients");
        Document selector;
        Cursor cursor(collection, selector);
        auto response = cursor.next();
        for (const auto& doc : response->documents()) {
            result.push_back(fromDocument(*doc));
        }
    } catch (...) {}
    return result;
}

bool PatientRepository::update(long long id, const models::Patient& patient) {
    try {
        auto collection = database_.getCollection("patients");
        Document selector;
        selector << "id" << id;
        Document updateDoc;
        updateDoc << "$set" << toDocument(patient);
        return collection.update(selector, updateDoc) > 0;
    } catch (...) {
        return false;
    }
}

bool PatientRepository::remove(long long id) {
    try {
        auto collection = database_.getCollection("patients");
        Document selector;
        selector << "id" << id;
        return collection.remove(selector) > 0;
    } catch (...) {
        return false;
    }
}

} // namespace db