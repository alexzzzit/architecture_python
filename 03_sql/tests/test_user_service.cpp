#include <gtest/gtest.h>
#include "services/user_service.h"
#include "models/user.h"
#include <libpq-fe.h>
#include <Poco/Environment.h>

// Очистка только тестовых записей
static void cleanupUsers() {
    std::string dbUrl = Poco::Environment::get("DATABASE_URL", "postgresql://medical_user:medical_password_2025@localhost:5432/medical_records");
    PGconn* conn = PQconnectdb(dbUrl.c_str());
    if (PQstatus(conn) == CONNECTION_OK) {
        PQexec(conn, "DELETE FROM users WHERE login LIKE 'test_%'");
        PQfinish(conn);
    }
}

class UserServiceTest : public ::testing::Test {
protected:
    void SetUp() override { cleanupUsers(); }
    void TearDown() override { cleanupUsers(); }
};

TEST_F(UserServiceTest, CreateUser_Success) {
    models::UserCreateRequest req;
    req.login = "test_create_ok";
    req.firstName = "John";
    req.lastName = "Doe";
    req.email = "test_create_ok@test.com";
    req.password = "password123";
    req.role = "patient";

    auto user = services::UserService::getInstance().create(req);
    ASSERT_TRUE(user.has_value());
    EXPECT_EQ(user.value().login, "test_create_ok");
    EXPECT_FALSE(user.value().id.empty());
}

TEST_F(UserServiceTest, CreateUser_DuplicateLogin) {
    models::UserCreateRequest req;
    req.login = "test_dup_login";
    req.firstName = "John";
    req.lastName = "Doe";
    req.email = "test_dup_login@test.com";
    req.password = "password123";
    req.role = "patient";

    auto res1 = services::UserService::getInstance().create(req);
    ASSERT_TRUE(res1.has_value()) << "Первое создание должно пройти";

    auto res2 = services::UserService::getInstance().create(req);
    ASSERT_FALSE(res2.has_value()) << "Дубликат логина должен отклоняться";
}

TEST_F(UserServiceTest, FindByLogin_Success) {
    models::UserCreateRequest req;
    req.login = "test_find_ok";
    req.firstName = "Jane";
    req.lastName = "Smith";
    req.email = "test_find_ok@test.com";
    req.password = "password123";
    req.role = "doctor";

    auto created = services::UserService::getInstance().create(req);
    ASSERT_TRUE(created.has_value());

    auto found = services::UserService::getInstance().findByLogin("test_find_ok");
    ASSERT_TRUE(found.has_value());
    EXPECT_EQ(found.value().firstName, "Jane");
}

TEST_F(UserServiceTest, FindByLogin_NotFound) {
    auto user = services::UserService::getInstance().findByLogin("nonexistent_xyz_999");
    ASSERT_FALSE(user.has_value());
}

TEST_F(UserServiceTest, SearchByName_Success) {
    models::UserCreateRequest req1, req2;
    req1.login = "test_search_ivan1"; req1.firstName = "Ivan"; req1.lastName = "Petrov";
    req1.email = "test_search_ivan1@test.com"; req1.password = "password123";

    req2.login = "test_search_ivan2"; req2.firstName = "Ivan"; req2.lastName = "Sidorov";
    req2.email = "test_search_ivan2@test.com"; req2.password = "password123";

    services::UserService::getInstance().create(req1);
    services::UserService::getInstance().create(req2);

    auto results = services::UserService::getInstance().searchByName("Ivan", "");
    EXPECT_GE(results.size(), 1);
}

TEST_F(UserServiceTest, ValidatePassword_Success) {
    models::UserCreateRequest req;
    req.login = "test_pwd_check";
    req.firstName = "Test";
    req.lastName = "User";
    req.email = "test_pwd_check@test.com";
    req.password = "my_secret_123";
    req.role = "patient";

    auto created = services::UserService::getInstance().create(req);
    ASSERT_TRUE(created.has_value());

    auto user = services::UserService::getInstance().findByLogin("test_pwd_check");
    ASSERT_TRUE(user.has_value());

    EXPECT_TRUE(services::UserService::getInstance().validatePassword(user.value(), "my_secret_123"));
    EXPECT_FALSE(services::UserService::getInstance().validatePassword(user.value(), "wrong_pass"));
}