#include "services/patient_service.h"
#include "integrations/postgres/patient_repository.h"
#include <Poco/Environment.h>

namespace services {

PatientService* PatientService::instance_ = nullptr;

PatientService& PatientService::getInstance() {
    if (!instance_) {
        instance_ = new PatientService();
    }
    return *instance_;
}

std::optional<models::Patient> PatientService::create(const models::PatientCreateRequest& req) {
    std::string dbUrl = Poco::Environment::get("DATABASE_URL", "postgresql://medical_user:medical_password_2025@localhost:5432/medical_records");
    return integrations::postgres::PatientRepository::create(req, dbUrl);
}

std::optional<models::Patient> PatientService::findById(const std::string& id) {
    std::string dbUrl = Poco::Environment::get("DATABASE_URL", "postgresql://medical_user:medical_password_2025@localhost:5432/medical_records");
    return integrations::postgres::PatientRepository::findById(id, dbUrl);
}

std::vector<models::Patient> PatientService::searchByFullName(const std::string& firstName, const std::string& lastName, const std::string& middleName) {
    std::string dbUrl = Poco::Environment::get("DATABASE_URL", "postgresql://medical_user:medical_password_2025@localhost:5432/medical_records");
    return integrations::postgres::PatientRepository::searchByFullName(firstName, lastName, middleName, dbUrl);
}

std::optional<models::MedicalRecord> PatientService::addRecord(const models::RecordCreateRequest& req) {
    std::string dbUrl = Poco::Environment::get("DATABASE_URL", "postgresql://medical_user:medical_password_2025@localhost:5432/medical_records");
    return integrations::postgres::PatientRepository::addRecord(req, dbUrl);
}

std::vector<models::MedicalRecord> PatientService::getPatientRecords(const std::string& patientId) {
    std::string dbUrl = Poco::Environment::get("DATABASE_URL", "postgresql://medical_user:medical_password_2025@localhost:5432/medical_records");
    return integrations::postgres::PatientRepository::getPatientRecords(patientId, dbUrl);
}

void PatientService::clear() {}
size_t PatientService::patientCount() const { return 0; }

} // namespace services