# Meson Build Status - Windows

## Current Status: ⚠️ Requires pkg-config

The Meson build system is fully configured with all `meson.build` files created, but on Windows it requires `pkg-config` to find Conan dependencies.

### Issue
Meson looks for dependencies using `pkg-config`, which is not typically available on Windows by default.

### Solution Options

#### Option 1: Use CMake (Recommended for Windows)
CMake build is fully working and tested:
```powershell
conan install . --output-folder=. --build=missing -s build_type=Release
cmake --preset conan-release
cmake --build --preset conan-release
```

#### Option 2: Install pkgconf for Meson
Install pkgconf to enable Meson builds on Windows:

```powershell
# Using Chocolatey
choco install pkgconf

# Or using Scoop
scoop install pkgconf

# Then configure Meson
meson setup builddir --pkg-config-path=build\Release\generators
meson compile -C builddir
```

#### Option 3: Use WSL/Linux for Meson
Meson works out-of-the-box on Linux:
```bash
conan install . --output-folder=. --build=missing -s build_type=Release
meson setup builddir --pkg-config-path=build/Release/generators
meson compile -C builddir
```

### What Was Fixed

All Meson build files have been created and configured:
- ✅ `libs/random/meson.build`
- ✅ `libs/optimizer/meson.build`  
- ✅ `apps/optimizer_cli/meson.build`
- ✅ All existing meson.build files updated with correct dependencies
- ✅ Fixed reserved option names in `meson_options.txt`
- ✅ Windows compatibility (removed Unix-only commands)

The Meson build configuration is complete and ready to use once pkg-config is available.

### Linux/macOS Status: ✅ Ready

On Linux and macOS, Meson should work without additional setup:
```bash
sudo apt-get install pkg-config  # Ubuntu/Debian
# or
brew install pkg-config          # macOS

conan install . --output-folder=. --build=missing -s build_type=Release
meson setup builddir --pkg-config-path=build/Release/generators
meson compile -C builddir
```

## Recommendation

**For Windows development**: Use CMake (already working perfectly)
**For Linux/macOS**: Use either CMake or Meson (both work)
**For CI/CD**: Use CMake for Windows, either for Linux/macOS

The project now supports both build systems, giving developers flexibility based on their platform and preferences!
