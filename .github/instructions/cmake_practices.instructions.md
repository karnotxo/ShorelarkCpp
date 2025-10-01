# CMake Best Practices

This guide outlines CMake best practices for the Shorelark C++ project.

## Project Structure

Our project uses the following structure for CMake files:

- Root `CMakeLists.txt` for global configuration and inclusion of subdirectories
- Subdirectory `CMakeLists.txt` files for each component (apps, libs)
- Further subdirectory `CMakeLists.txt` files for specific libraries or applications

## Modern CMake Approach

Always use modern CMake with targets and properties instead of the old variable-based approach.

### Bad (Old CMake Style):
```cmake
include_directories(include)
add_definitions(-DSOME_DEFINE)
add_executable(myapp main.cc)
target_link_libraries(myapp ${SOME_LIBRARY})
```

### Good (Modern CMake Style):
```cmake
add_executable(myapp main.cc)
target_include_directories(myapp PUBLIC include)
target_compile_definitions(myapp PRIVATE SOME_DEFINE)
target_link_libraries(myapp PRIVATE some_library)
```

## Target Visibility

Use appropriate visibility modifiers:

- `PRIVATE`: Dependencies only used in implementation files (.cpp)
- `PUBLIC`: Dependencies used in both implementation and headers
- `INTERFACE`: Dependencies only used in headers

```cmake
target_include_directories(mylib 
    PUBLIC 
        include  # Headers exposed to users
    PRIVATE 
        src      # Internal headers
)

target_link_libraries(mylib
    PUBLIC
        dependency_exposed_in_header
    PRIVATE
        internal_dependency
)
```

## Library Creation

Create libraries properly with STATIC or SHARED:

```cmake
add_library(neural_network STATIC
    src/neuron.cc
    src/layer.cc
    src/network.cc
)
```

Use object libraries for code reuse without duplication:

```cmake
add_library(common_objects OBJECT
    src/utilities.cc
    src/logging.cc
)

add_library(app_lib STATIC
    $<TARGET_OBJECTS:common_objects>
    src/app_specific.cc
)
```

## Interface Libraries

Use interface libraries for header-only libraries:

```cmake
add_library(header_only_lib INTERFACE)
target_include_directories(header_only_lib INTERFACE include)
```

## Finding Packages

Use `find_package` to locate dependencies:

```cmake
find_package(Catch2 REQUIRED)
find_package(spdlog REQUIRED)
```

## Testing

Enable testing properly:

```cmake
enable_testing()
include(CTest)
include(Catch)

add_executable(neural_network_tests
    test/neuron_test.cc
    test/layer_test.cc
)

target_link_libraries(neural_network_tests
    PRIVATE
    neural_network
    Catch2::Catch2WithMain
)

catch_discover_tests(neural_network_tests)
```

## Options and Configuration

Use options for build configuration:

```cmake
option(ENABLE_TESTING "Enable testing" ON)
option(BUILD_SHARED_LIBS "Build shared libraries" OFF)
```

## Target Properties

Set properties on targets:

```cmake
set_target_properties(myapp PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)
```

## Output Directories

Configure output directories:

```cmake
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
```

## Using Conan

Include Conan toolchain file:

```cmake
if(EXISTS "${CMAKE_BINARY_DIR}/generators/conan_toolchain.cmake")
    include("${CMAKE_BINARY_DIR}/generators/conan_toolchain.cmake")
endif()
```

## Managing Compiler Flags

Set compiler flags appropriately:

```cmake
if(MSVC)
    target_compile_options(myapp PRIVATE /W4 /WX /utf-8)
else()
    target_compile_options(myapp PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()
```

## Add Subdirectories

Add subdirectories properly:

```cmake
add_subdirectory(libs)
add_subdirectory(apps)
```

## Install Rules

Define install rules:

```cmake
install(TARGETS mylib
    EXPORT mylib-targets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include
)

install(DIRECTORY include/
    DESTINATION include
    FILES_MATCHING PATTERN "*.h"
)
```

## Export Package

Export package configuration:

```cmake
include(CMakePackageConfigHelpers)

write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/mylibConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

install(EXPORT mylib-targets
    FILE mylibTargets.cmake
    NAMESPACE mylib::
    DESTINATION lib/cmake/mylib
)
```

## Presets

Use CMake presets for consistent builds:

```json
{
  "version": 3,
  "configurePresets": [
    {
      "name": "debug",
      "displayName": "Debug",
      "generator": "Ninja",
      "binaryDir": "${sourceDir}/build/debug",
      "cacheVariables": {
        "CMAKE_BUILD_TYPE": "Debug",
        "CMAKE_EXPORT_COMPILE_COMMANDS": "ON"
      }
    },
    {
      "name": "release",
      "displayName": "Release",
      "generator": "Ninja",
      "binaryDir": "${sourceDir}/build/release",
      "cacheVariables": {
        "CMAKE_BUILD_TYPE": "Release",
        "CMAKE_EXPORT_COMPILE_COMMANDS": "ON"
      }
    }
  ]
}
```

## Common Patterns for Shorelark

### Component Library Pattern

Each major component in the Shorelark project (e.g., neural_network, genetic_algorithm) should follow this pattern:

```cmake
# libs/component_name/CMakeLists.txt
add_library(component_name
    src/file1.cc
    src/file2.cc
)

target_include_directories(component_name
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(component_name
    PUBLIC
        dependency1
    PRIVATE
        dependency2
)

if(ENABLE_TESTING)
    add_executable(component_name_tests
        test/file1_test.cc
        test/file2_test.cc
    )
    
    target_link_libraries(component_name_tests
        PRIVATE
            component_name
            Catch2::Catch2WithMain
    )
    
    catch_discover_tests(component_name_tests)
endif()
```

### Application Pattern

Applications should follow this pattern:

```cmake
# apps/app_name/CMakeLists.txt
add_executable(app_name
    src/main.cc
    src/file1.cc
    src/file2.cc
)

target_include_directories(app_name
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(app_name
    PRIVATE
        component1
        component2
        external_lib
)
```

## Best Practices Summary

1. Use modern CMake with targets and properties
2. Use proper visibility specifiers (PRIVATE, PUBLIC, INTERFACE)
3. Structure the project logically with subdirectories
4. Configure compiler flags consistently
5. Use options for configurable features
6. Set output directories clearly
7. Use testing properly with CTest
8. Create proper installation rules
9. Use Conan integration through the toolchain file
10. Use CMake presets for build configurations
