# Conan 2 Package Management

This guide outlines best practices for using Conan 2 for dependency management in the Shorelark C++ project.

## Project Setup

Our project uses Conan 2 for dependency management with a `conanfile.py` in the root directory.

## Installing Conan 2

Ensure you have Conan 2.x installed:

```bash
pip install conan
conan --version  # Should show version 2.x
```

## Using the Conanfile

Our `conanfile.py` specifies project dependencies, build requirements, and other configuration.

## Basic Workflow

### 1. Install Dependencies

To install dependencies:

```powershell
# Create a build directory if it doesn't exist
mkdir -p build
cd build

# Install dependencies
conan install .. --build=missing
```

### 2. CMake Integration

The Conan-generated files are integrated with CMake in the main `CMakeLists.txt`:

```cmake
if(EXISTS "${CMAKE_BINARY_DIR}/generators/conan_toolchain.cmake")
    include("${CMAKE_BINARY_DIR}/generators/conan_toolchain.cmake")
elseif(EXISTS "${CMAKE_BINARY_DIR}/../generators/conan_toolchain.cmake")
    include("${CMAKE_BINARY_DIR}/../generators/conan_toolchain.cmake")
endif()
```

### 3. Find Packages

After including the toolchain file, we can find packages using `find_package`:

```cmake
find_package(Catch2 REQUIRED)
find_package(spdlog REQUIRED)
find_package(imgui REQUIRED)
```

## Conan Profiles

Use Conan profiles to manage different build configurations:

### Create Default Profile

```powershell
conan profile detect  # Creates a default profile
```

### Create Custom Profiles

Create profiles in `~/.conan2/profiles/` or use the `--profile` option.

Example `debug` profile:
```ini
[settings]
arch=x86_64
build_type=Debug
compiler=msvc
compiler.cppstd=17
compiler.runtime=dynamic
compiler.version=193
os=Windows

[options]
```

## Using Multiple Build Types

```powershell
# Debug build
conan install .. -s build_type=Debug -of=build/debug

# Release build
conan install .. -s build_type=Release -of=build/release
```

## Conanfile.py Structure

Our `conanfile.py` specifies project requirements, configuration, and build options:

```python
from conan import ConanFile
from conan.tools.cmake import cmake_layout

class ShorelarkCppConan(ConanFile):
    name = "shorelark_cpp"
    version = "1.0.0"
    
    # Binary configuration
    settings = "os", "compiler", "build_type", "arch"
    options = {
        "shared": [True, False],
        "fPIC": [True, False],
    }
    default_options = {
        "shared": False,
        "fPIC": True,
    }
    
    # Dependencies
    requires = [
        "catch2/3.3.2",
        "spdlog/1.11.0",
        "imgui/1.89.7",
        "glfw/3.3.8",
        "glew/2.2.0",
        "transwarp/2.2.3",
        "asio/1.28.0",
        "tinyxml2/9.0.0",
        "concurrentqueue/1.0.4",
        "nlohmann_json/3.11.2",
        "range-v3/0.12.0",
        "span-lite/0.10.3",
        "tl-expected/1.1.0",
        "freetype/2.13.0",
        "stb/cci.20220909",
        "args/6.4.6",
        "date/3.0.1",
        "lunasvg/2.3.5",
        "tomlplusplus/3.3.0",
        "units/0.7.2",
        "fmt/10.0.0",
        "libenvpp/0.1.0",
        "fakeit/2.3.1"
    ]
    
    # Build dependencies
    tool_requires = [
        "cmake/3.25.3",
        "ninja/1.11.1"
    ]
    
    def layout(self):
        cmake_layout(self)
        
    def generate(self):
        # Generate build files
        pass
        
    def build(self):
        # Build the project
        pass
        
    def package(self):
        # Define what to include when packaging
        pass
        
    def package_info(self):
        # Define variables when this package is consumed
        pass
```

## Adding Dependencies

To add a new dependency:

1. Add it to the `requires` list in `conanfile.py`
2. Run `conan install ..` to update dependencies
3. Use `find_package` in CMake to find the package

Example adding a new dependency:
```python
# In conanfile.py
requires = [
    # ... existing dependencies
    "boost/1.81.0",
]
```

```cmake
# In CMakeLists.txt
find_package(Boost REQUIRED)
```

## Versioning

Use specific versions for reproducible builds:

```python
requires = [
    "spdlog/1.11.0",  # Prefer specific versions
    "fmt/[>8.0.0 <11.0.0]"  # Or use version ranges
]
```

## Customizing Package Options

Set options for dependencies:

```python
default_options = {
    "shared": False,
    "fPIC": True,
    "spdlog:shared": False,
    "imgui:docking_branch": True
}
```

## Handling Conflicts

When dependencies conflict, use `override` to specify which version to use:

```python
from conan import ConanFile

class MyConan(ConanFile):
    requires = [
        "library_a/1.0",  # Uses fmt/8.0
        "library_b/2.0",  # Uses fmt/9.0
    ]
    
    tool_requires = []
    
    def requirements(self):
        # Resolve conflict
        self.requires("fmt/9.0", override=True)
```

## Generators

Use appropriate generators for your build system:

```python
def generate(self):
    cmake = CMakeToolchain(self)
    cmake.generate()
    
    deps = CMakeDeps(self)
    deps.generate()
```

## Lock Files

Create lock files for reproducible builds:

```powershell
# Create lock file
conan lock create conanfile.py --lockfile=conan.lock

# Use lock file
conan install .. --lockfile=conan.lock
```

## Local Development of Dependencies

For local development of dependencies, use `conan workspace`:

```powershell
# Create a workspace file (conanws.yml)
# Then use it
conan workspace install conanws.yml
```

## Best Practices Summary

1. Use specific versions for dependencies
2. Create and use profiles for different build configurations
3. Use lockfiles for reproducible builds
4. Customize dependency options when needed
5. Use the toolchain integration for seamless CMake builds
6. Keep the conanfile.py organized and well-commented
7. Use tool_requires for build tools
8. Specify compiler settings in profiles
9. Use different build directories for different configurations
