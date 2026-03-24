#include <gtest/gtest.h>
#include "services/patient_service.h"
#include "models/patient.h"

// Тестовый класс для PatientService
class PatientServiceTest : public ::testing::Test {
protected:
    // Очищает хранилище перед каждым тестом
    void SetUp() override {
        services::PatientService::getInstance().clear();
    }
    
    // Очищает хранилище после каждого теста
    void TearDown() override {
        services::PatientService::getInstance().clear();
    }
};

// Тест успешной регистрации пациента
TEST_F(PatientServiceTest, CreatePatient_Success) {
    models::PatientCreateRequest req;
    req.userId = "usr_1";
    req.firstName = "Ivan";
    req.lastName = "Petrov";
    req.middleName = "Ivanovich";
    req.birthDate = "1990-05-15";
    req.phone = "+79991234567";
    req.insuranceNumber = "SNILS-123456789";

    auto patient = services::PatientService::getInstance().create(req);

    ASSERT_TRUE(patient.has_value());
    EXPECT_EQ(patient.value().firstName, "Ivan");
    EXPECT_EQ(patient.value().lastName, "Petrov");
    EXPECT_EQ(patient.value().middleName, "Ivanovich");
    EXPECT_EQ(patient.value().birthDate, "1990-05-15");
    EXPECT_EQ(patient.value().insuranceNumber, "SNILS-123456789");
    EXPECT_FALSE(patient.value().id.empty());
}

// Тест поиска пациента по ID
TEST_F(PatientServiceTest, FindPatientById_Success) {
    models::PatientCreateRequest req;
    req.userId = "usr_1";
    req.firstName = "Maria";
    req.lastName = "Sidorova";
    req.middleName = "";
    req.birthDate = "1985-03-20";
    req.phone = "+79997654321";
    req.insuranceNumber = "SNILS-987654321";

    auto created = services::PatientService::getInstance().create(req);
    ASSERT_TRUE(created.has_value());

    auto found = services::PatientService::getInstance().findById(created.value().id);
    ASSERT_TRUE(found.has_value());
    EXPECT_EQ(found.value().id, created.value().id);
}

// Тест поиска пациентов по ФИО
TEST_F(PatientServiceTest, SearchByFullName_Success) {
    models::PatientCreateRequest req1, req2, req3;
    
    req1.userId = "usr_1";
    req1.firstName = "Ivan";
    req1.lastName = "Petrov";
    req1.middleName = "Ivanovich";
    req1.birthDate = "1990-01-01";
    req1.phone = "+79991111111";
    req1.insuranceNumber = "SNILS-111";

    req2.userId = "usr_2";
    req2.firstName = "Ivan";
    req2.lastName = "Sidorov";
    req2.middleName = "Petrovich";
    req2.birthDate = "1991-02-02";
    req2.phone = "+79992222222";
    req2.insuranceNumber = "SNILS-222";

    req3.userId = "usr_3";
    req3.firstName = "Petr";
    req3.lastName = "Petrov";
    req3.middleName = "Ivanovich";
    req3.birthDate = "1992-03-03";
    req3.phone = "+79993333333";
    req3.insuranceNumber = "SNILS-333";

    services::PatientService::getInstance().create(req1);
    services::PatientService::getInstance().create(req2);
    services::PatientService::getInstance().create(req3);

    // Поиск по имени
    auto results = services::PatientService::getInstance().searchByFullName("Ivan", "", "");
    EXPECT_EQ(results.size(), 2);

    // Поиск по фамилии
    results = services::PatientService::getInstance().searchByFullName("", "Petrov", "");
    EXPECT_EQ(results.size(), 2);

    // Поиск по полному ФИО
    results = services::PatientService::getInstance().searchByFullName("Ivan", "Petrov", "Ivanovich");
    EXPECT_EQ(results.size(), 1);
}

// Тест добавления медицинской записи пациенту
TEST_F(PatientServiceTest, AddRecord_Success) {
    models::PatientCreateRequest preq;
    preq.userId = "usr_1";
    preq.firstName = "Test";
    preq.lastName = "Patient";
    preq.middleName = "";
    preq.birthDate = "1990-01-01";
    preq.phone = "+79990000000";
    preq.insuranceNumber = "SNILS-000";

    auto patient = services::PatientService::getInstance().create(preq);
    ASSERT_TRUE(patient.has_value());

    models::RecordCreateRequest rreq;
    rreq.patientId = patient.value().id;
    rreq.doctorId = "usr_doctor";
    rreq.diagnosis = "Acute bronchitis";
    rreq.treatment = "Antibiotics, rest";
    rreq.notes = "Follow up in 7 days";

    auto record = services::PatientService::getInstance().addRecord(rreq);

    ASSERT_TRUE(record.has_value());
    EXPECT_EQ(record.value().patientId, patient.value().id);
    EXPECT_EQ(record.value().diagnosis, "Acute bronchitis");
    EXPECT_FALSE(record.value().code.empty());
}

// Тест получения истории записей пациента
TEST_F(PatientServiceTest, GetPatientRecords_Success) {
    models::PatientCreateRequest preq;
    preq.userId = "usr_1";
    preq.firstName = "Test";
    preq.lastName = "Patient";
    preq.middleName = "";
    preq.birthDate = "1990-01-01";
    preq.phone = "+79990000000";
    preq.insuranceNumber = "SNILS-000";

    auto patient = services::PatientService::getInstance().create(preq);
    ASSERT_TRUE(patient.has_value());

    models::RecordCreateRequest rreq1, rreq2;
    rreq1.patientId = patient.value().id;
    rreq1.doctorId = "usr_doctor1";
    rreq1.diagnosis = "Diagnosis 1";
    rreq1.treatment = "Treatment 1";
    
    rreq2.patientId = patient.value().id;
    rreq2.doctorId = "usr_doctor2";
    rreq2.diagnosis = "Diagnosis 2";
    rreq2.treatment = "Treatment 2";

    services::PatientService::getInstance().addRecord(rreq1);
    services::PatientService::getInstance().addRecord(rreq2);

    auto records = services::PatientService::getInstance().getPatientRecords(patient.value().id);
    EXPECT_EQ(records.size(), 2);
}

// Тест подсчёта количества пациентов
TEST_F(PatientServiceTest, PatientCount) {
    EXPECT_EQ(services::PatientService::getInstance().patientCount(), 0);

    models::PatientCreateRequest req;
    req.userId = "usr_1";
    req.firstName = "Test";
    req.lastName = "Patient";
    req.middleName = "";
    req.birthDate = "1990-01-01";
    req.phone = "+79990000000";
    req.insuranceNumber = "SNILS-000";

    services::PatientService::getInstance().create(req);
    EXPECT_EQ(services::PatientService::getInstance().patientCount(), 1);

    req.userId = "usr_2";
    services::PatientService::getInstance().create(req);
    EXPECT_EQ(services::PatientService::getInstance().patientCount(), 2);
}
