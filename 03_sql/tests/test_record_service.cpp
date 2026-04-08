#include <gtest/gtest.h>
#include "services/record_service.h"
#include "models/medical_record.h"
#include <libpq-fe.h>
#include <Poco/Environment.h>

class RecordServiceTest : public ::testing::Test {
protected:

};

TEST_F(RecordServiceTest, FindByCode_Success) {
    auto found = services::RecordService::getInstance().findByCode("REC-2024-001");
    ASSERT_TRUE(found.has_value());
    EXPECT_EQ(found.value().code, "REC-2024-001");
    EXPECT_FALSE(found.value().diagnosis.empty());
}

TEST_F(RecordServiceTest, FindByCode_NotFound) {
    auto record = services::RecordService::getInstance().findByCode("NONEXISTENT_TEST_CODE_XYZ");
    ASSERT_FALSE(record.has_value());
}

TEST_F(RecordServiceTest, FindByPatientId_Success) {
    auto records = services::RecordService::getInstance().findByPatientId("b0000001-0000-4000-8000-000000000001");
    EXPECT_GE(records.size(), 1);
}