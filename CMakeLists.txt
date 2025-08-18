# needed for CMake presets
cmake_minimum_required (VERSION 3.25)

# Include the Conan toolchain file if specified
if(EXISTS "${CMAKE_BINARY_DIR}/generators/conan_toolchain.cmake")
    include("${CMAKE_BINARY_DIR}/generators/conan_toolchain.cmake")
elseif(EXISTS "${CMAKE_BINARY_DIR}/../generators/conan_toolchain.cmake")
    include("${CMAKE_BINARY_DIR}/../generators/conan_toolchain.cmake")
endif()

set_property(GLOBAL PROPERTY USE_FOLDERS OFF)

if ("${CMAKE_BINARY_DIR}" STREQUAL "${CMAKE_SOURCE_DIR}")
    message(FATAL_ERROR "In-source builds are not allowed.
        Please create a subfolder and use `cmake ..` inside it.
        NOTE: cmake creates CMakeCache.txt and CMakeFiles/*.
              Remove them, or cmake will refuse to work.")
endif()

# Build options
option(ENABLE_TESTING "Enable a Unit Testing build." ON)
option(ENABLE_CLANG_TIDY "Enable clang-tidy checks during build" ON)
option(ENABLE_CLANG_FORMAT "Enable clang-format checks during build" ON)
option(ENABLE_IWYU "Enable include-what-you-use checks" OFF)
option(ENABLE_CPPCHECK "Enable cppcheck static analysis" OFF)

# Find tools
find_program(CLANG_TIDY_EXE NAMES clang-tidy)
find_program(CLANG_FORMAT_EXE NAMES clang-format)
find_program(IWYU_EXE NAMES include-what-you-use iwyu)
find_program(CPPCHECK_EXE NAMES cppcheck)

# Configure clang-tidy
if(ENABLE_CLANG_TIDY)
    if(CLANG_TIDY_EXE)
        set(CMAKE_CXX_CLANG_TIDY
            ${CLANG_TIDY_EXE};
            -extra-arg=-Wno-unknown-warning-option;
			-extra-arg=$<IF:$<CXX_COMPILER_ID:MSVC>,/EHsc,-fcxx-exceptions>;
			-extra-arg=$<IF:$<CXX_COMPILER_ID:MSVC>,/std:c++17,-std=c++17>;
			-extra-arg=$<$<CXX_COMPILER_ID:MSVC>:/permissive->;
            -p=${CMAKE_BINARY_DIR})
        message(STATUS "clang-tidy found: ${CLANG_TIDY_EXE}")
    else()
        message(WARNING "clang-tidy not found!")
        set(CMAKE_CXX_CLANG_TIDY "")
    endif()
endif()

# Configure clang-format
if(ENABLE_CLANG_FORMAT)
    if(CLANG_FORMAT_EXE)
        message(STATUS "clang-format found: ${CLANG_FORMAT_EXE}")
        file(GLOB_RECURSE ALL_SOURCE_FILES
            ${PROJECT_SOURCE_DIR}/src/*.cc
            ${PROJECT_SOURCE_DIR}/src/*.h
            ${PROJECT_SOURCE_DIR}/include/*.h
            ${PROJECT_SOURCE_DIR}/test/*.cc
            ${PROJECT_SOURCE_DIR}/test/*.h
            ${PROJECT_SOURCE_DIR}/apps/*.cc
            ${PROJECT_SOURCE_DIR}/apps/*.h
            ${PROJECT_SOURCE_DIR}/libs/*.cc
            ${PROJECT_SOURCE_DIR}/libs/*.h
        )
        add_custom_target(
            format
            COMMAND ${CLANG_FORMAT_EXE}
            -style=file
            -i
            ${ALL_SOURCE_FILES}
            COMMENT "Formatting source files..."
        )

        # Add format-check target
        add_custom_target(
            format-check
            COMMAND ${CLANG_FORMAT_EXE}
            -style=file
            --dry-run
            --Werror
            ${ALL_SOURCE_FILES}
            COMMENT "Checking source formatting..."
        )
    else()
        message(WARNING "clang-format not found!")
    endif()
endif()

# Configure include-what-you-use
if(ENABLE_IWYU)
    if(IWYU_EXE)
        set(CMAKE_CXX_INCLUDE_WHAT_YOU_USE ${IWYU_EXE})
        message(STATUS "include-what-you-use found: ${IWYU_EXE}")
    else()
        message(WARNING "include-what-you-use not found!")
    endif()
endif()

# Configure cppcheck
if(ENABLE_CPPCHECK)
    if(CPPCHECK_EXE)
        set(CMAKE_CXX_CPPCHECK
            ${CPPCHECK_EXE}
            --enable=all
            --inconclusive
            --force
            --inline-suppr
            --suppressions-list=${CMAKE_SOURCE_DIR}/.cppcheck-suppressions)
        message(STATUS "cppcheck found: ${CPPCHECK_EXE}")
    else()
        message(WARNING "cppcheck not found!")
    endif()
endif()

set(CSHORELARK_VERSION 1.0.0)
project(cshorelark LANGUAGES C CXX VERSION ${CSHORELARK_VERSION})

message(STATUS "CMAKE_CXX_COMPILER: ${CMAKE_CXX_COMPILER}")
message(STATUS "Compiler ID: ${CMAKE_CXX_COMPILER_ID} (${CMAKE_CXX_COMPILER_VERSION})")

# Basic build settings
set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_DEBUG_POSTFIX "d" CACHE STRING "Filename postfix for libraries under DEBUG configuration")

set(CXX_STANDARD "${CMAKE_CXX_STANDARD}")

if (CMAKE_CXX_STANDARD LESS 17)
    message(FATAL_ERROR "A c++17 enabled compiler is needed for building.
                 NOTE: cmake creates CMakeCache.txt and CMakeFiles/*.
                       Remove them, or cmake will refuse to work.")
endif()

# Add directory of CMake-Resources to the cmake module path.
list( APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake" )
# Now it can be included like this:
# include(CMakeRC)

# Compiler flags
if(MSVC)
    add_compile_options(/W4 /utf-8 /Zc:__cplusplus /permissive- /EHsc)
    add_definitions(
        /D _CRT_SECURE_NO_WARNINGS
        /D NOMINMAX
        /D STRICT
        /D WIN32_LEAN_AND_MEAN
        /D VC_EXTRALEAN
    )
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

add_definitions(-D_USE_MATH_DEFINES)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

# Find required packages
find_package(Threads REQUIRED)
find_package(imgui REQUIRED)
find_package(glfw3 REQUIRED)
find_package(GLEW REQUIRED)
find_package(transwarp REQUIRED)
find_package(asio REQUIRED)
find_package(tinyxml2 REQUIRED)
find_package(concurrentqueue REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(range-v3 REQUIRED)
find_package(span-lite REQUIRED)
find_package(tl-expected REQUIRED)
#find_package(string-view-lite REQUIRED)
#find_package(tl-optional REQUIRED)
find_package(freetype REQUIRED)
find_package(stb REQUIRED)
find_package(args REQUIRED)
find_package(date REQUIRED)
find_package(lunasvg REQUIRED)
find_package(tomlplusplus REQUIRED)
find_package(units REQUIRED)
find_package(fmt REQUIRED)
find_package(spdlog REQUIRED)
find_package(Catch2 REQUIRED)
find_package(libenvpp REQUIRED)
find_package(fakeit REQUIRED)

include(Catch)

# Enable CTest and testing so subprojects can register tests
if(ENABLE_TESTING)
    include(CTest)
    enable_testing()
endif()

message(STATUS "Building with CMake ${CMAKE_VERSION} and C++ standard ${CMAKE_CXX_STANDARD}")
message(STATUS "Output path: ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

# add_subdirectory(include)

# Add libraries first
add_subdirectory(libs)

# Then add applications
add_subdirectory(apps)

# Doxygen Documentation
find_package(Doxygen)
if(DOXYGEN_FOUND)
    set(DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/docs/Doxyfile.in)
    set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

    configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)

    add_custom_target(docs
        COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Generating API documentation with Doxygen"
        VERBATIM)
endif()

option(BUILD_DOCS "Build documentation" OFF)

if(BUILD_DOCS)
    find_package(Doxygen REQUIRED)
    find_package(Sphinx REQUIRED)

    # Configure Doxygen
    set(DOXYGEN_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/docs/api)
    set(DOXYGEN_GENERATE_HTML YES)
    set(DOXYGEN_GENERATE_XML YES)  # Required for Sphinx integration

    # Create Doxygen target
    doxygen_add_docs(doxygen
        ${PROJECT_SOURCE_DIR}/include
        ${PROJECT_SOURCE_DIR}/libs
        ${PROJECT_SOURCE_DIR}/apps
        ${CMAKE_CURRENT_SOURCE_DIR}/README.md
        COMMENT "Generating API documentation with Doxygen"
    )

    # Configure Sphinx
    set(SPHINX_SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/docs/sphinx)
    set(SPHINX_BUILD ${CMAKE_CURRENT_BINARY_DIR}/docs/sphinx)
    set(SPHINX_INDEX_FILE ${SPHINX_BUILD}/index.html)

    # Create Sphinx target
    add_custom_target(sphinx ALL
        COMMAND ${SPHINX_EXECUTABLE} -b html
        -Dbreathe_projects.CShorelark=${DOXYGEN_OUTPUT_DIRECTORY}/xml
        ${SPHINX_SOURCE} ${SPHINX_BUILD}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Generating documentation with Sphinx"
        DEPENDS doxygen
    )

    # Create combined docs target
    add_custom_target(docs DEPENDS doxygen sphinx
        COMMENT "Generating complete documentation"
    )
endif()
