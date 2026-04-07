#include <gtest/gtest.h>
#include "services/record_service.h"
#include "models/medical_record.h"

// Тестовый класс для RecordService
class RecordServiceTest : public ::testing::Test {
protected:
    // Очищает хранилище записей перед каждым тестом
    void SetUp() override {
        services::RecordService::getInstance().clear();
    }
    
    // Очищает хранилище записей после каждого теста
    void TearDown() override {
        services::RecordService::getInstance().clear();
    }
};

// Тест успешного создания медицинской записи
TEST_F(RecordServiceTest, CreateRecord_Success) {
    models::RecordCreateRequest req;
    req.patientId = "pat_1";
    req.doctorId = "usr_doctor";
    req.diagnosis = "Hypertension";
    req.treatment = "Medication, diet";
    req.notes = "Monitor blood pressure";

    auto record = services::RecordService::getInstance().create(req);

    ASSERT_TRUE(record.has_value());
    EXPECT_EQ(record.value().patientId, "pat_1");
    EXPECT_EQ(record.value().doctorId, "usr_doctor");
    EXPECT_EQ(record.value().diagnosis, "Hypertension");
    EXPECT_EQ(record.value().treatment, "Medication, diet");
    EXPECT_FALSE(record.value().code.empty());
    EXPECT_GT(record.value().createdAt, 0);
}

// Тест поиска записи по коду
TEST_F(RecordServiceTest, FindByCode_Success) {
    models::RecordCreateRequest req;
    req.patientId = "pat_1";
    req.doctorId = "usr_doctor";
    req.diagnosis = "Test Diagnosis";
    req.treatment = "Test Treatment";
    req.notes = "";

    auto created = services::RecordService::getInstance().create(req);
    ASSERT_TRUE(created.has_value());

    auto found = services::RecordService::getInstance().findByCode(created.value().code);
    ASSERT_TRUE(found.has_value());
    EXPECT_EQ(found.value().code, created.value().code);
    EXPECT_EQ(found.value().diagnosis, "Test Diagnosis");
}

// Тест поиска несуществующей записи
TEST_F(RecordServiceTest, FindByCode_NotFound) {
    auto record = services::RecordService::getInstance().findByCode("NONEXISTENT-CODE");
    ASSERT_FALSE(record.has_value());
}

// Тест уникальности кодов записей
TEST_F(RecordServiceTest, RecordCode_Uniqueness) {
    models::RecordCreateRequest req;
    req.patientId = "pat_1";
    req.doctorId = "usr_doctor";
    req.diagnosis = "Test";
    req.treatment = "Test";
    req.notes = "";

    auto record1 = services::RecordService::getInstance().create(req);
    auto record2 = services::RecordService::getInstance().create(req);

    ASSERT_TRUE(record1.has_value());
    ASSERT_TRUE(record2.has_value());
    EXPECT_NE(record1.value().code, record2.value().code);
}

// Тест подсчёта количества записей
TEST_F(RecordServiceTest, Count_Records) {
    EXPECT_EQ(services::RecordService::getInstance().count(), 0);

    models::RecordCreateRequest req;
    req.patientId = "pat_1";
    req.doctorId = "usr_doctor";
    req.diagnosis = "Test";
    req.treatment = "Test";
    req.notes = "";

    services::RecordService::getInstance().create(req);
    EXPECT_EQ(services::RecordService::getInstance().count(), 1);

    services::RecordService::getInstance().create(req);
    EXPECT_EQ(services::RecordService::getInstance().count(), 2);
}
