cmake_minimum_required(VERSION 3.28)

set(CMAKE_EXPERIMENTAL_CXX_IMPORT_STD "d0edc3af-4c50-42ea-a356-e2862fe7a444")

project(visual-novel-engine)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT CMAKE_GENERATOR MATCHES "^Ninja")
    message(FATAL_ERROR "This project uses `import std;`, which currently requires a Ninja generator. Configure with `cmake --preset clang-homebrew`.")
endif()

set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_MODULE_STD ON)
set(CMAKE_CXX_SCAN_FOR_MODULES ON)

if(NOT 26 IN_LIST CMAKE_CXX_COMPILER_IMPORT_STD)
    message(FATAL_ERROR "The configured toolchain does not provide `import std` for C++26. Configure with `cmake --preset clang-homebrew`.")
endif()

set(CXX_FLAGS
    -fsanitize=address
    -fsanitize=undefined
    -fstack-protector-strong
    -Wall
    -Wcast-align
    -Wextra
    -Wformat
    -Wformat-security
    -Wno-shadow
    -Wno-unused-result
    -Wnull-dereference
    -Wredundant-decls
    -Wreturn-type
    -Wstrict-overflow
    -Wuninitialized
    -Wvla
)

list(JOIN CXX_FLAGS " " CMAKE_CXX_FLAGS)

file(GLOB SOURCES src/*.cpp)
add_executable(visual-novel-engine ${SOURCES})
