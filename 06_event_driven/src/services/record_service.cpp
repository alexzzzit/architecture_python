#include "services/record_service.h"
#include "integrations/postgres/record_repository.h"
#include <Poco/Environment.h>

namespace services {

RecordService* RecordService::instance_ = nullptr;

RecordService& RecordService::getInstance() {
    if (!instance_) {
        instance_ = new RecordService();
    }
    return *instance_;
}

std::optional<models::MedicalRecord> RecordService::create(const models::RecordCreateRequest& req) {
    std::string dbUrl = Poco::Environment::get("DATABASE_URL", "postgresql://medical_user:medical_password_2025@localhost:5432/medical_records");
    return integrations::postgres::RecordRepository::create(req, dbUrl);
}

std::optional<models::MedicalRecord> RecordService::findByCode(const std::string& code) {
    std::string dbUrl = Poco::Environment::get("DATABASE_URL", "postgresql://medical_user:medical_password_2025@localhost:5432/medical_records");
    return integrations::postgres::RecordRepository::findByCode(code, dbUrl);
}

std::vector<models::MedicalRecord> RecordService::findByPatientId(const std::string& patientId) {
    std::string dbUrl = Poco::Environment::get("DATABASE_URL", "postgresql://medical_user:medical_password_2025@localhost:5432/medical_records");
    return integrations::postgres::RecordRepository::findByPatientId(patientId, dbUrl);
}

size_t RecordService::count() const {
    std::string dbUrl = Poco::Environment::get("DATABASE_URL", "postgresql://medical_user:medical_password_2025@localhost:5432/medical_records");
    return integrations::postgres::RecordRepository::count(dbUrl);
}

void RecordService::clear() {}

} // namespace services