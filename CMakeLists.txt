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

set(MODULE_COMPAT_CXX_FLAGS
    -fstack-protector-strong
    -fsanitize=address
    -fsanitize=undefined
)

# `import std` builds an internal std module target that does not inherit
# `target_compile_options()`, so module-compatible flags must be global.
list(JOIN MODULE_COMPAT_CXX_FLAGS " " VNE_MODULE_COMPAT_CXX_FLAGS)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${VNE_MODULE_COMPAT_CXX_FLAGS}")
string(STRIP "${CMAKE_CXX_FLAGS}" CMAKE_CXX_FLAGS)

file(GLOB SOURCES CONFIGURE_DEPENDS src/*.cpp)

add_executable(visual-novel-engine ${SOURCES})
set_target_properties(visual-novel-engine PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}"
)
target_compile_options(visual-novel-engine PRIVATE ${COMMON_CXX_FLAGS})
install(TARGETS visual-novel-engine RUNTIME DESTINATION bin)
