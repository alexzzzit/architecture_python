#pragma once
#include <string>
#include <ctime>

namespace models {

struct Patient {
    std::string id;
    std::string userId;
    std::string firstName;
    std::string lastName;
    std::string middleName;
    std::string birthDate;
    std::string phone;
    std::string insuranceNumber;
    std::string snils;
    std::string address;
    time_t createdAt = 0;
};

struct PatientCreateRequest {
    std::string userId;
    std::string firstName;
    std::string lastName;
    std::string middleName;
    std::string birthDate;
    std::string phone;
    std::string insuranceNumber;
    std::string snils;
    std::string address;
};

} // namespace models