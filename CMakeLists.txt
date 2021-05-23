cmake_minimum_required(VERSION 3.15)
project(Alsong_cpp
        VERSION 0.0.1
        DESCRIPTION "Alsong api wrapper"
        HOMEPAGE_URL https://github.com/organization/Alsong-cpp
        LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(mbedcrypto)
include_directories(mbedcrypto/include)

find_package(OpenSSL REQUIRED)
include_directories(${OPENSSL_INCLUDE_DIR})

include_directories(cpp-httplib)
include_directories(date/include)

include_directories(simdjson)

include_directories(include)
add_library(Alsong_cpp simdjson/simdjson.cpp src/library.cpp)
target_link_libraries(Alsong_cpp ${OPENSSL_LIBRARIES})