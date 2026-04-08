#include <gtest/gtest.h>
#include "services/patient_service.h"
#include "models/patient.h"
#include "services/user_service.h"
#include "models/user.h"
#include <libpq-fe.h>
#include <Poco/Environment.h>

static void cleanupPatients() {
    std::string dbUrl = Poco::Environment::get("DATABASE_URL", "postgresql://medical_user:medical_password_2025@localhost:5432/medical_records");
    PGconn* conn = PQconnectdb(dbUrl.c_str());
    if (PQstatus(conn) == CONNECTION_OK) {
        PQexec(conn, "DELETE FROM medical_records WHERE code LIKE 'TEST_%'");
        PQexec(conn, "DELETE FROM patients WHERE policy_number LIKE 'TEST_%'");
        PQexec(conn, "DELETE FROM users WHERE login LIKE 'test_pat_%'");
        PQfinish(conn);
    }
}

class PatientServiceTest : public ::testing::Test {
protected:
    void SetUp() override { cleanupPatients(); }
    void TearDown() override { cleanupPatients(); }
};

TEST_F(PatientServiceTest, CreatePatient_Success) {
    models::UserCreateRequest ureq;
    ureq.login = "test_pat_owner";
    ureq.firstName = "Owner";
    ureq.lastName = "Test";
    ureq.email = "test_pat_owner@test.com";
    ureq.password = "password123";
    ureq.role = "patient";

    auto user = services::UserService::getInstance().create(ureq);
    if (!user.has_value()) {
        GTEST_SKIP() << "Пропуск: не удалось создать пользователя-владельца";
    }

    models::PatientCreateRequest preq;
    preq.userId = user.value().id;
    preq.firstName = "Ivan";
    preq.lastName = "Petrov";
    preq.middleName = "Ivanovich";
    preq.birthDate = "1990-05-15";
    preq.phone = "+79991234567";
    preq.insuranceNumber = "TEST_PAT_001";

    auto patient = services::PatientService::getInstance().create(preq);
    ASSERT_TRUE(patient.has_value());
    EXPECT_EQ(patient.value().firstName, "Ivan");
}

TEST_F(PatientServiceTest, FindPatientById_Success) {
    // Используем реальные данные из data.sql
    auto found = services::PatientService::getInstance().findById("b0000001-0000-4000-8000-000000000001");
    ASSERT_TRUE(found.has_value());
    EXPECT_FALSE(found.value().id.empty());
}

TEST_F(PatientServiceTest, SearchByFullName_Success) {
    // Ищем по данным из data.sql (patient_ivanov: Дмитрий Иванов)
    auto results = services::PatientService::getInstance().searchByFullName("Дмитрий", "Иванов", "");
    EXPECT_GE(results.size(), 1);
}

TEST_F(PatientServiceTest, GetPatientRecords_Success) {
    // У пациента из data.sql есть записи
    auto records = services::PatientService::getInstance().getPatientRecords("b0000001-0000-4000-8000-000000000001");
    EXPECT_GE(records.size(), 1);
}