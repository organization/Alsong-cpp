cmake_minimum_required(VERSION 3.15)
project(alsongpp
        VERSION 0.0.1
        DESCRIPTION "Alsong api wrapper"
        HOMEPAGE_URL https://github.com/organization/Alsong-cpp
        LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

find_package(OpenSSL REQUIRED)
include_directories(${OPENSSL_INCLUDE_DIR})

include_directories(cpp-httplib)
add_subdirectory(date)
include_directories(date/include)

include_directories(json/include)

include_directories(include)
add_library(${PROJECT_NAME} src/library.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE date::date ${OPENSSL_LIBRARIES})
