#pragma once

#include <string>
#include <Poco/Timestamp.h>
#include <Poco/MongoDB/Document.h>

namespace models {

struct Address {
    std::string city;
    std::string street;
    std::string building;
    std::string apartment;

    Poco::MongoDB::Document toBson() const;
    static Address fromBson(const Poco::MongoDB::Document& doc);
};

struct EmergencyContact {
    std::string name;
    std::string phone;
    std::string relation;

    Poco::MongoDB::Document toBson() const;
    static EmergencyContact fromBson(const Poco::MongoDB::Document& doc);
};

class Patient {
public:
    Patient() = default;

    long long id = 0;
    long long userId = 0;
    std::string policyNumber;
    std::string snils;
    std::string firstName;
    std::string lastName;
    std::string middleName;
    Poco::Timestamp birthDate;
    std::string phone;
    Address address;
    EmergencyContact emergencyContact;
    Poco::Timestamp createdAt;
    Poco::Timestamp updatedAt;

    Poco::MongoDB::Document toBson() const;
    static Patient fromBson(const Poco::MongoDB::Document& doc);
};

} // namespace models