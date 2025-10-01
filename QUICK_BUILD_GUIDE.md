# Quick Build Guide - ShorelarkCpp

## Prerequisites

### Windows
```powershell
# Install Python 3.x from python.org
# Install Visual Studio 2019+ with C++ Development Tools

# Install build tools
pip install conan meson ninja

# Configure Conan profile
conan profile detect --force
```

### Linux/macOS
```bash
# Install build tools
pip install conan meson ninja

# Install system dependencies (Ubuntu/Debian)
sudo apt-get install build-essential libgl1-mesa-dev xorg-dev

# Configure Conan profile
conan profile detect --force
```

## Building with CMake (Recommended)

```powershell
# 1. Install dependencies
conan install . --output-folder=. --build=missing -s build_type=Release

# 2. Configure
cmake --preset conan-release

# 3. Build
cmake --build --preset conan-release

# 4. Run
.\build\Release\bin\simulation-ui.exe     # GUI app
.\build\Release\bin\optimizer-cli.exe     # CLI optimizer
```

## Building with Meson

```powershell
# 1. Install dependencies
conan install . --output-folder=. --build=missing -s build_type=Release

# 2. Configure
meson setup builddir --pkg-config-path=build\Release\generators

# 3. Build
meson compile -C builddir

# 4. Run
.\builddir\apps\simulation_ui\simulation_ui.exe
.\builddir\apps\optimizer_cli\optimizer_cli.exe
```

## Running Tests

### CMake
```powershell
cd build
ctest --preset conan-release --output-on-failure
```

### Meson
```powershell
meson test -C builddir --verbose
```

## Common Issues

### Issue: "conan: command not found"
**Solution**: Install Conan with `pip install conan`

### Issue: "meson: command not found"
**Solution**: Install Meson with `pip install meson ninja`

### Issue: "CMake: command not found"
**Solution**: Install CMake from https://cmake.org/download/

### Issue: Conan can't find compiler
**Solution**: Run from Visual Studio Developer Command Prompt on Windows

### Issue: Missing pkg-config files
**Solution**: Make sure you used the correct `--output-folder=.` path with Conan (it generates the build folder automatically)

### Issue: "Dependency 'xxx' not found"
**Solution**: Check that conanfile.py has the dependency and run `conan install` again

## Quick Commands Cheatsheet

```powershell
# Clean build (CMake)
cmake --build --preset conan-release --target clean

# Clean build (Meson)
meson compile -C builddir --clean

# Reconfigure (CMake)
cmake --preset conan-release --fresh

# Reconfigure (Meson)
meson setup --reconfigure builddir

# Install (CMake)
cmake --build --preset conan-release --target install

# Install (Meson)
meson install -C builddir

# Format code (if clang-format is installed)
clang-format -i libs/**/*.cc libs/**/*.h apps/**/*.cc apps/**/*.h

# Run specific test (Meson)
meson test -C builddir neural_network_tests
```

## Project Structure

```
ShorelarkCpp/
├── apps/
│   ├── simulation_ui/      # GUI application with ImGui
│   └── optimizer_cli/      # Command-line optimizer
├── libs/
│   ├── random/            # Random number generation
│   ├── neural_network/    # Neural network implementation
│   ├── genetic_algorithm/ # Genetic algorithm library
│   ├── simulation/        # Core simulation logic
│   └── optimizer/         # Training optimizer
├── bindings/              # ImGui platform bindings
├── build/                 # CMake build output
├── builddir/             # Meson build output (if using Meson)
└── docs/                 # Documentation
```

## Configuration Files

- `CMakeLists.txt` - CMake build configuration
- `meson.build` - Meson build configuration
- `conanfile.py` - Conan dependencies
- `CMakeUserPresets.json` - CMake presets
- `meson_options.txt` - Meson build options

## Build Options

### CMake Options
```powershell
-DENABLE_TESTING=ON        # Enable tests
-DENABLE_SANITIZERS=ON     # Enable address/undefined sanitizers
-DBUILD_DOCS=ON            # Build documentation
-DUSE_LTO=ON               # Link-time optimization
```

### Meson Options
```powershell
-Dbuild_tests=true         # Enable tests
-Dbuild_docs=true          # Build documentation
-Denable_sanitizers=true   # Enable sanitizers
-Dbuildtype=release        # Release build
-Dbuildtype=debug          # Debug build
```

## Documentation

Build documentation with Doxygen and Sphinx:

```powershell
# CMake
cmake --build --preset conan-release --target docs

# Meson (requires -Dbuild_docs=true)
meson compile -C builddir docs
```

Documentation will be generated in:
- CMake: `build/docs/`
- Meson: `builddir/docs/`

## Support

- GitHub Issues: https://github.com/karnotxo/ShorelarkCpp/issues
- Original Project: https://github.com/patryk27/shorelark
- Learning Series: https://pwy.io/posts/learning-to-fly-pt1/
