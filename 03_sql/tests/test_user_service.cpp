#include <gtest/gtest.h>
#include "services/user_service.h"
#include "models/user.h"

// Тестовый класс для UserService
// Наследуется от ::testing::Test для использования фикстур
class UserServiceTest : public ::testing::Test {
protected:
    // Выполняется перед каждым тестом
    // Очищает хранилище пользователей для изоляции тестов
    void SetUp() override {
        services::UserService::getInstance().clear();
    }
    
    // Выполняется после каждого теста
    // Очищает хранилище для следующего теста
    void TearDown() override {
        services::UserService::getInstance().clear();
    }
};

// Тест успешного создания пользователя
TEST_F(UserServiceTest, CreateUser_Success) {
    models::UserCreateRequest req;
    req.login = "test_user";
    req.firstName = "John";
    req.lastName = "Doe";
    req.email = "john@example.com";
    req.password = "password123";
    req.role = "patient";

    auto user = services::UserService::getInstance().create(req);

    // Проверяем что пользователь создан
    ASSERT_TRUE(user.has_value());
    EXPECT_EQ(user.value().login, "test_user");
    EXPECT_EQ(user.value().firstName, "John");
    EXPECT_EQ(user.value().lastName, "Doe");
    EXPECT_EQ(user.value().email, "john@example.com");
    EXPECT_EQ(user.value().role, "patient");
    EXPECT_FALSE(user.value().id.empty());
    EXPECT_GT(user.value().createdAt, 0);
}

// Тест создания пользователя с дублирующимся логином
TEST_F(UserServiceTest, CreateUser_DuplicateLogin) {
    models::UserCreateRequest req;
    req.login = "duplicate_user";
    req.firstName = "John";
    req.lastName = "Doe";
    req.email = "john@example.com";
    req.password = "password123";
    req.role = "patient";

    // Первое создание успешно
    auto result1 = services::UserService::getInstance().create(req);
    // Второе создание должно вернуться nullopt
    auto result2 = services::UserService::getInstance().create(req);

    ASSERT_TRUE(result1.has_value());
    ASSERT_FALSE(result2.has_value());
}

// Тест поиска пользователя по логину
TEST_F(UserServiceTest, FindByLogin_Success) {
    models::UserCreateRequest req;
    req.login = "find_me";
    req.firstName = "Jane";
    req.lastName = "Smith";
    req.email = "jane@example.com";
    req.password = "password123";
    req.role = "doctor";

    services::UserService::getInstance().create(req);

    auto user = services::UserService::getInstance().findByLogin("find_me");

    ASSERT_TRUE(user.has_value());
    EXPECT_EQ(user.value().login, "find_me");
    EXPECT_EQ(user.value().firstName, "Jane");
}

// Тест поиска несуществующего пользователя
TEST_F(UserServiceTest, FindByLogin_NotFound) {
    auto user = services::UserService::getInstance().findByLogin("nonexistent");
    ASSERT_FALSE(user.has_value());
}

// Тест поиска пользователей по имени и фамилии
TEST_F(UserServiceTest, SearchByName_Success) {
    models::UserCreateRequest req1, req2, req3;
    
    req1.login = "user1";
    req1.firstName = "John";
    req1.lastName = "Smith";
    req1.email = "john1@example.com";
    req1.password = "password123";
    
    req2.login = "user2";
    req2.firstName = "John";
    req2.lastName = "Doe";
    req2.email = "john2@example.com";
    req2.password = "password123";
    
    req3.login = "user3";
    req3.firstName = "Jane";
    req3.lastName = "Smith";
    req3.email = "jane@example.com";
    req3.password = "password123";

    services::UserService::getInstance().create(req1);
    services::UserService::getInstance().create(req2);
    services::UserService::getInstance().create(req3);

    // Поиск по имени
    auto results = services::UserService::getInstance().searchByName("John", "");
    EXPECT_EQ(results.size(), 2);

    // Поиск по фамилии
    results = services::UserService::getInstance().searchByName("", "Smith");
    EXPECT_EQ(results.size(), 2);

    // Поиск по имени и фамилии
    results = services::UserService::getInstance().searchByName("John", "Smith");
    EXPECT_EQ(results.size(), 1);
}

// Тест валидации пароля
TEST_F(UserServiceTest, ValidatePassword_Success) {
    models::UserCreateRequest req;
    req.login = "password_test";
    req.firstName = "Test";
    req.lastName = "User";
    req.email = "test@example.com";
    req.password = "secure_password";
    req.role = "patient";

    auto user = services::UserService::getInstance().create(req);
    ASSERT_TRUE(user.has_value());

    // Правильный пароль
    EXPECT_TRUE(services::UserService::getInstance().validatePassword(user.value(), "secure_password"));
    // Неправильный пароль
    EXPECT_FALSE(services::UserService::getInstance().validatePassword(user.value(), "wrong_password"));
}

// Тест подсчёта количества пользователей
TEST_F(UserServiceTest, Count_Users) {
    EXPECT_EQ(services::UserService::getInstance().count(), 0);

    models::UserCreateRequest req;
    req.login = "user1";
    req.firstName = "Test";
    req.lastName = "User";
    req.email = "test1@example.com";
    req.password = "password123";
    
    services::UserService::getInstance().create(req);
    EXPECT_EQ(services::UserService::getInstance().count(), 1);

    req.login = "user2";
    req.email = "test2@example.com";
    services::UserService::getInstance().create(req);
    EXPECT_EQ(services::UserService::getInstance().count(), 2);
}
