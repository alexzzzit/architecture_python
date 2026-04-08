cmake_minimum_required(VERSION 3.15)
project(medical_records_api VERSION 1.0.0 LANGUAGES CXX C)

# Стандарт C++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Тип сборки
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

find_package(Poco REQUIRED COMPONENTS Net NetSSL JSON Foundation)
find_package(PostgreSQL REQUIRED)

option(BUILD_TESTS "Build Google Test tests" ON)

if(BUILD_TESTS)
    include(FetchContent)
    FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
        DOWNLOAD_EXTRACT_TIMESTAMP true
    )
    if(WIN32)
        set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    endif()
    FetchContent_MakeAvailable(googletest)
endif()

include_directories(${CMAKE_SOURCE_DIR}/includes)
include_directories(${Poco_INCLUDE_DIRS})
include_directories(${PostgreSQL_INCLUDE_DIRS})

set(LIB_SOURCES
    src/handlers/user_handler.cpp
    src/handlers/patient_handler.cpp
    src/handlers/record_handler.cpp
    src/handlers/auth_handler.cpp
    src/services/user_service.cpp
    src/services/patient_service.cpp
    src/services/record_service.cpp
    src/services/notification_service.cpp
    src/integrations/insurance_integration.cpp
    src/integrations/laboratory_integration.cpp
    src/integrations/postgres/connection_pool.cpp
    src/integrations/postgres/user_repository.cpp
    src/integrations/postgres/patient_repository.cpp
    src/integrations/postgres/record_repository.cpp
    src/cache/cache_manager.cpp
    src/auth/jwt_manager.cpp
    src/utils/json_response.cpp
)

add_library(medical_lib STATIC ${LIB_SOURCES})
target_include_directories(medical_lib PUBLIC
    ${CMAKE_SOURCE_DIR}/includes
    ${Poco_INCLUDE_DIRS}
    ${PostgreSQL_INCLUDE_DIRS}
)
target_link_libraries(medical_lib PUBLIC
    Poco::Net
    Poco::NetSSL
    Poco::JSON
    Poco::Foundation
    PostgreSQL::PostgreSQL
)

add_executable(${PROJECT_NAME} src/main.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE medical_lib)
target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic)

if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

install(TARGETS ${PROJECT_NAME} DESTINATION bin)
install(DIRECTORY docs/ DESTINATION docs)