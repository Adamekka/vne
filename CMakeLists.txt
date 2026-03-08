cmake_minimum_required(VERSION 3.28)

set(CMAKE_EXPERIMENTAL_CXX_IMPORT_STD "d0edc3af-4c50-42ea-a356-e2862fe7a444")

project(visual-novel-engine LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT CMAKE_GENERATOR MATCHES "^Ninja")
    message(FATAL_ERROR
        "This project uses `import std;`, which currently requires the Ninja generator. "
        "Configure with one of the Ninja presets from `CMakePresets.json`."
    )
endif()

set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_MODULE_STD ON)
set(CMAKE_CXX_SCAN_FOR_MODULES ON)

set(COMMON_CXX_FLAGS
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

set(SANITIZER_FLAGS
    -fsanitize=address
    -fsanitize=undefined
)

file(GLOB SOURCES CONFIGURE_DEPENDS src/*.cpp)
add_executable(visual-novel-engine ${SOURCES})
target_compile_options(visual-novel-engine PRIVATE ${COMMON_CXX_FLAGS} ${SANITIZER_FLAGS})
target_link_options(visual-novel-engine PRIVATE ${SANITIZER_FLAGS})
install(TARGETS visual-novel-engine RUNTIME DESTINATION bin)
