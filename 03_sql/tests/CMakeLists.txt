# tests/CMakeLists.txt
cmake_minimum_required(VERSION 3.15)

# Исполняемый файл тестов
add_executable(medical_records_tests
    test_user_service.cpp
    test_patient_service.cpp
    test_record_service.cpp
)

target_include_directories(medical_records_tests PRIVATE
    ${CMAKE_SOURCE_DIR}/includes
)

target_link_libraries(medical_records_tests PRIVATE
    medical_lib
    GTest::gtest_main
    GTest::gmock
)

# Включение тестирования
enable_testing()
add_test(NAME MedicalRecordsTests COMMAND medical_records_tests)

# Устанавливаем бинарник тестов в корень build/
set_target_properties(medical_records_tests PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)