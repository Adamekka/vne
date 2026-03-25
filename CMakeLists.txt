cmake_minimum_required(VERSION 3.28)

# MARK: Project

project(visual-novel-engine LANGUAGES C CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# MARK: Tooling

set(VNE_ROOT_COMPILE_COMMANDS_PATH "${CMAKE_SOURCE_DIR}/compile_commands.json")

if(IS_SYMLINK "${VNE_ROOT_COMPILE_COMMANDS_PATH}")
    file(REMOVE "${VNE_ROOT_COMPILE_COMMANDS_PATH}")
endif()

if(NOT EXISTS "${VNE_ROOT_COMPILE_COMMANDS_PATH}")
    file(CREATE_LINK
        "${CMAKE_BINARY_DIR}/compile_commands.json"
        "${VNE_ROOT_COMPILE_COMMANDS_PATH}"
        SYMBOLIC
        RESULT VNE_CREATE_LINK_RESULT
    )

    if(NOT VNE_CREATE_LINK_RESULT STREQUAL "0")
        message(WARNING
            "Unable to create `${VNE_ROOT_COMPILE_COMMANDS_PATH}` -> "
            "`${CMAKE_BINARY_DIR}/compile_commands.json`: ${VNE_CREATE_LINK_RESULT}"
        )
    endif()
else()
    message(STATUS
        "Keeping existing `${VNE_ROOT_COMPILE_COMMANDS_PATH}` because it is not a symlink."
    )
endif()

set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# MARK: Warnings

set(VNE_CXX_FLAGS
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
    -fstack-protector-strong
    -fsanitize=address
    -fsanitize=undefined
)

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

file(GLOB_RECURSE VNE_SOURCES CONFIGURE_DEPENDS
    src/*.cpp
    src/*.cppm
    src/*.ixx
)
list(FILTER VNE_SOURCES EXCLUDE REGEX "/main\\.cpp$")

add_executable(visual-novel-engine)

target_sources(visual-novel-engine
    PRIVATE
        src/main.cpp
        ${VNE_SOURCES}
)

set_target_properties(visual-novel-engine PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}"
)
target_compile_options(visual-novel-engine PRIVATE ${VNE_CXX_FLAGS})
target_link_options(visual-novel-engine PRIVATE -fsanitize=address -fsanitize=undefined)
target_link_libraries(visual-novel-engine PRIVATE raylib)
install(TARGETS visual-novel-engine RUNTIME DESTINATION bin)
