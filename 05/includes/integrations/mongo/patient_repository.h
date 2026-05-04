#pragma once

#include <Poco/MongoDB/Database.h>
#include <Poco/MongoDB/Document.h>
#include <optional>
#include <vector>
#include "models/patient.h"

namespace db {

class PatientRepository {
public:
    explicit PatientRepository(Poco::MongoDB::Database& database);

    bool create(const models::Patient& patient);
    std::optional<models::Patient> findById(long long id);
    std::optional<models::Patient> findByUserId(long long userId);
    std::optional<models::Patient> findByPolicyNumber(const std::string& policyNumber);
    std::optional<models::Patient> findBySnils(const std::string& snils);
    std::vector<models::Patient> findByNamePattern(const std::string& firstName, const std::string& lastName);
    std::vector<models::Patient> findAll();
    bool update(long long id, const models::Patient& patient);
    bool remove(long long id);

private:
    Poco::MongoDB::Document toDocument(const models::Patient& patient) const;
    models::Patient fromDocument(const Poco::MongoDB::Document& doc) const;

    Poco::MongoDB::Database& database_;
};

} // namespace db