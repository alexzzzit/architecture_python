#include "models/user.h"
#include <Poco/DateTimeParser.h>
#include <Poco/DateTimeFormat.h>
#include <Poco/DateTimeFormatter.h>

namespace models {

Poco::MongoDB::Document User::toBson() const {
    Poco::MongoDB::Document doc;
    doc << "id" << id
        << "login" << login
        << "email" << email
        << "password_hash" << passwordHash
        << "role" << roleToString(role)
        << "is_active" << isActive
        << "created_at" << Poco::DateTimeFormatter::format(createdAt, Poco::DateTimeFormat::ISO8601_FORMAT)
        << "updated_at" << Poco::DateTimeFormatter::format(updatedAt, Poco::DateTimeFormat::ISO8601_FORMAT);
    return doc;
}

User User::fromBson(const Poco::MongoDB::Document& doc) {
    User user;
    user.id = doc.get<long long>("id");
    user.login = doc.get<std::string>("login");
    user.email = doc.get<std::string>("email");
    user.passwordHash = doc.get<std::string>("password_hash");
    user.role = roleFromString(doc.get<std::string>("role"));
    user.isActive = doc.get<bool>("is_active");
    
    if (doc.has("created_at")) {
        user.createdAt = Poco::DateTimeParser::parse(Poco::DateTimeFormat::ISO8601_FORMAT, doc.get<std::string>("created_at"));
    }
    if (doc.has("updated_at")) {
        user.updatedAt = Poco::DateTimeParser::parse(Poco::DateTimeFormat::ISO8601_FORMAT, doc.get<std::string>("updated_at"));
    }
    return user;
}

std::string User::roleToString(Role role) {
    switch (role) {
        case Role::PATIENT: return "patient";
        case Role::DOCTOR: return "doctor";
        case Role::ADMIN: return "admin";
        case Role::LAB_TECHNICIAN: return "lab_technician";
        default: return "patient";
    }
}

User::Role User::roleFromString(const std::string& role) {
    if (role == "patient") return Role::PATIENT;
    if (role == "doctor") return Role::DOCTOR;
    if (role == "admin") return Role::ADMIN;
    if (role == "lab_technician") return Role::LAB_TECHNICIAN;
    return Role::PATIENT;
}

} // namespace models