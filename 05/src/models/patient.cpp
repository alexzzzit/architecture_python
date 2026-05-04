#include "models/patient.h"
#include <Poco/DateTimeParser.h>
#include <Poco/DateTimeFormat.h>
#include <Poco/DateTimeFormatter.h>

namespace models {

Poco::MongoDB::Document Address::toBson() const {
    Poco::MongoDB::Document doc;
    doc << "city" << city << "street" << street << "building" << building << "apartment" << apartment;
    return doc;
}

Address Address::fromBson(const Poco::MongoDB::Document& doc) {
    Address addr;
    addr.city = doc.get<std::string>("city");
    addr.street = doc.get<std::string>("street");
    addr.building = doc.get<std::string>("building");
    addr.apartment = doc.get<std::string>("apartment");
    return addr;
}

Poco::MongoDB::Document EmergencyContact::toBson() const {
    Poco::MongoDB::Document doc;
    doc << "name" << name << "phone" << phone << "relation" << relation;
    return doc;
}

EmergencyContact EmergencyContact::fromBson(const Poco::MongoDB::Document& doc) {
    EmergencyContact ec;
    ec.name = doc.get<std::string>("name");
    ec.phone = doc.get<std::string>("phone");
    ec.relation = doc.get<std::string>("relation");
    return ec;
}

Poco::MongoDB::Document Patient::toBson() const {
    Poco::MongoDB::Document doc;
    doc << "id" << id
        << "user_id" << userId
        << "policy_number" << policyNumber
        << "snils" << snils
        << "first_name" << firstName
        << "last_name" << lastName
        << "middle_name" << middleName
        << "birth_date" << Poco::DateTimeFormatter::format(birthDate, Poco::DateTimeFormat::ISO8601_FORMAT)
        << "phone" << phone
        << "address" << address.toBson()
        << "emergency_contact" << emergencyContact.toBson()
        << "created_at" << Poco::DateTimeFormatter::format(createdAt, Poco::DateTimeFormat::ISO8601_FORMAT)
        << "updated_at" << Poco::DateTimeFormatter::format(updatedAt, Poco::DateTimeFormat::ISO8601_FORMAT);
    return doc;
}

Patient Patient::fromBson(const Poco::MongoDB::Document& doc) {
    Patient patient;
    patient.id = doc.get<long long>("id");
    patient.userId = doc.get<long long>("user_id");
    patient.policyNumber = doc.get<std::string>("policy_number");
    patient.snils = doc.get<std::string>("snils");
    patient.firstName = doc.get<std::string>("first_name");
    patient.lastName = doc.get<std::string>("last_name");
    patient.middleName = doc.get<std::string>("middle_name");
    patient.birthDate = Poco::DateTimeParser::parse(Poco::DateTimeFormat::ISO8601_FORMAT, doc.get<std::string>("birth_date"));
    patient.phone = doc.get<std::string>("phone");
    patient.address = Address::fromBson(doc.getObject("address"));
    patient.emergencyContact = EmergencyContact::fromBson(doc.getObject("emergency_contact"));
    patient.createdAt = Poco::DateTimeParser::parse(Poco::DateTimeFormat::ISO8601_FORMAT, doc.get<std::string>("created_at"));
    patient.updatedAt = Poco::DateTimeParser::parse(Poco::DateTimeFormat::ISO8601_FORMAT, doc.get<std::string>("updated_at"));
    return patient;
}

} // namespace models
