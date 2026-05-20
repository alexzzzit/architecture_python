cmake_minimum_required(VERSION 3.16)
project(medical_records_api VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

find_package(Poco REQUIRED COMPONENTS Foundation JSON Util Net)
find_package(PkgConfig REQUIRED)

pkg_check_modules(RDKAFKA REQUIRED IMPORTED_TARGET rdkafka)
pkg_check_modules(LIBPQ REQUIRED IMPORTED_TARGET libpq)

# === Библиотека событий ===
add_library(events
    src/events/kafka_producer.cpp
    src/events/kafka_consumer.cpp
)

target_include_directories(events PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/includes
    ${Poco_INCLUDE_DIRS}
    ${RDKAFKA_INCLUDE_DIRS}
)

target_link_libraries(events PUBLIC
    ${Poco_LIBRARIES}
    ${RDKAFKA_LDFLAGS}
)

# === Основное приложение ===
add_executable(medical_records_api
    src/main.cpp
    
    src/handlers/auth_handler.cpp
    src/handlers/user_handler.cpp
    src/handlers/patient_handler.cpp
    src/handlers/record_handler.cpp
    
    src/services/user_service.cpp
    src/services/patient_service.cpp
    src/services/record_service.cpp
    src/services/notification_service.cpp
    
    src/integrations/postgres/connection_pool.cpp
    src/integrations/postgres/user_repository.cpp
    src/integrations/postgres/patient_repository.cpp
    src/integrations/postgres/record_repository.cpp
    src/integrations/insurance_integration.cpp
    src/integrations/laboratory_integration.cpp
    
    src/auth/jwt_manager.cpp
    src/utils/json_response.cpp
)

target_include_directories(medical_records_api PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/includes
    ${Poco_INCLUDE_DIRS}
    ${LIBPQ_INCLUDE_DIRS}
)

target_link_libraries(medical_records_api PRIVATE
    events
    ${Poco_LIBRARIES}
    ${LIBPQ_LDFLAGS}
)

install(TARGETS medical_records_api DESTINATION bin)