cmake_minimum_required(VERSION 3.28)

set(CMAKE_EXPERIMENTAL_CXX_IMPORT_STD "d0edc3af-4c50-42ea-a356-e2862fe7a444")

# MARK: Project

project(visual-novel-engine LANGUAGES C CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# MARK: Generator

if(NOT CMAKE_GENERATOR MATCHES "^Ninja")
    message(FATAL_ERROR
        "This project uses `import std;`, which currently requires the Ninja generator. "
        "Configure with one of the Ninja presets from `CMakePresets.json`."
    )
endif()

set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# MARK: Warnings

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

# MARK: Module Compatibility

# `import std` builds an internal std module target that does not inherit
# `target_compile_options()`, so module-compatible flags must be global.
list(JOIN MODULE_COMPAT_CXX_FLAGS " " VNE_MODULE_COMPAT_CXX_FLAGS)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${VNE_MODULE_COMPAT_CXX_FLAGS}")
string(STRIP "${CMAKE_CXX_FLAGS}" CMAKE_CXX_FLAGS)

# MARK: Dependencies

set(RAYLIB_VERSION 5.5)
find_package(raylib ${RAYLIB_VERSION} QUIET)

if(NOT raylib_FOUND)
    include(FetchContent)

    FetchContent_Declare(
        raylib
        DOWNLOAD_EXTRACT_TIMESTAMP OFF
        URL "https://github.com/raysan5/raylib/archive/refs/tags/${RAYLIB_VERSION}.tar.gz"
    )

    FetchContent_MakeAvailable(raylib)
endif()

# MARK: Target

file(GLOB SOURCES CONFIGURE_DEPENDS src/*.cpp)

add_executable(visual-novel-engine ${SOURCES})
set_target_properties(visual-novel-engine PROPERTIES
    CXX_MODULE_STD ON
    CXX_SCAN_FOR_MODULES ON
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}"
)
target_compile_options(visual-novel-engine PRIVATE ${COMMON_CXX_FLAGS})
target_link_libraries(visual-novel-engine PRIVATE raylib)
install(TARGETS visual-novel-engine RUNTIME DESTINATION bin)
