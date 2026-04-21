cmake_minimum_required(VERSION 3.16)
project(medical_server VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Предпочитать статические библиотеки
set(BUILD_SHARED_LIBS OFF)

find_package(Poco REQUIRED COMPONENTS
    Net
    NetSSL
    Foundation
    Util
    JSON
)

add_executable(medical_server
    src/main.cpp
)

target_include_directories(medical_server PRIVATE
    ${CMAKE_SOURCE_DIR}/includes
)

# Статическая линковка
target_link_libraries(medical_server PRIVATE
    Poco::Net
    Poco::NetSSL
    Poco::Foundation
    Poco::Util
    Poco::JSON
)

# Флаги для статической линковки
target_link_options(medical_server PRIVATE
    -static-libgcc
    -static-libstdc++
)

target_compile_options(medical_server PRIVATE
    -Wall
    -Wextra
    -pedantic
)

install(TARGETS medical_server DESTINATION bin)