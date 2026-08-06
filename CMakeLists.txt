cmake_minimum_required(VERSION 4.0.2)

project(VeilExchange VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Boost REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(Threads REQUIRED)

add_executable(
    VeilExchange
    src/main.cpp
    src/Application.cpp
    src/Connection.cpp
    src/Message.cpp
)

target_include_directories(
    VeilExchange
    PRIVATE
        ${PROJECT_SOURCE_DIR}/include
)

target_compile_definitions(
    VeilExchange
    PRIVATE
        BOOST_ERROR_CODE_HEADER_ONLY
)

target_link_libraries(
    VeilExchange
    PRIVATE
        OpenSSL::SSL
        OpenSSL::Crypto
        Threads::Threads
)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(
        VeilExchange
        PRIVATE
            -Wall
            -Wextra
            -Wpedantic
    )
endif()
