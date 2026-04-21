#pragma once

#include <string>
#include <vector>
#include <optional>

namespace models {

struct UserCreateRequest {
    std::string login;
    std::string email;
    std::string password;
    std::string role;
};

struct UserUpdateRequest {
    std::optional<std::string> email;
    std::optional<std::string> password;
    std::optional<bool> isActive;
};

struct PatientCreateRequest {
    long long userId;
    std::string policyNumber;
    std::string snils;
    std::string firstName;
    std::string lastName;
    std::string middleName;
    std::string birthDate;
    std::string phone;
};

struct RecordCreateRequest {
    std::string code;
    long long patientId;
    long long doctorId;
    std::string diagnosis;
    std::vector<std::string> symptoms;
    std::string status;
    std::string visitDate;
};

struct RecordUpdateRequest {
    std::optional<std::string> diagnosis;
    std::optional<std::vector<std::string>> symptoms;
    std::optional<std::string> status;
};

} // namespace models