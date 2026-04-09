cmake_minimum_required(VERSION 3.15)
project(MedicalRecordsAPI VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Poco REQUIRED COMPONENTS Net NetSSL JSON Foundation)
find_package(PostgreSQL REQUIRED)

file(GLOB_RECURSE LIB_SOURCES "src/*.cpp")
list(REMOVE_ITEM LIB_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp")

add_library(medical_lib STATIC ${LIB_SOURCES})

target_include_directories(medical_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/includes)

target_link_libraries(medical_lib PUBLIC
    Poco::Net
    Poco::NetSSL
    Poco::JSON
    Poco::Foundation
    PostgreSQL::PostgreSQL
)

add_executable(medical_records_api src/main.cpp)
target_link_libraries(medical_records_api PRIVATE medical_lib)

install(TARGETS medical_records_api DESTINATION bin)