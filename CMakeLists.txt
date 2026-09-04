cmake_minimum_required(VERSION 3.20)

project(UE5ServMan
    VERSION 1.0
    LANGUAGES CXX
)


set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


#
# Release default
#

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()


#
# External libraries
#

include(FetchContent)


#
# cpp-httplib
#

FetchContent_Declare(
    httplib
    GIT_REPOSITORY https://github.com/yhirose/cpp-httplib.git
    GIT_TAG v0.18.3
)


#
# nlohmann/json
#

FetchContent_Declare(
    json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.11.3
)


FetchContent_MakeAvailable(
    httplib
    json
)


#
# Source files
#

set(SOURCES

    src/main.cpp

    src/helpers.cpp

    src/server_manager.cpp

    src/process_manager.cpp
)


#
# Executable
#

add_executable(
    UE5ServMan
    ${SOURCES}
)


#
# Include directories
#

target_include_directories(
    UE5ServMan
    PRIVATE

    ${CMAKE_CURRENT_SOURCE_DIR}/src
)


#
# Libraries
#

target_link_libraries(
    UE5ServMan
    PRIVATE

    httplib::httplib

    nlohmann_json::nlohmann_json
)


#
# Linux pthread
#

find_package(Threads REQUIRED)


target_link_libraries(
    UE5ServMan
    PRIVATE

    Threads::Threads
)


#
# Compiler options
#

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")

    target_compile_options(
        UE5ServMan
        PRIVATE

        -Wall
        -Wextra
        -Wpedantic
        -O3
        -fvisibility=hidden
    )

endif()


#
# Release linker optimization
#

if(CMAKE_BUILD_TYPE STREQUAL "Release")

    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")

        target_link_options(
            UE5ServMan
            PRIVATE

            -s
        )

    endif()

endif()