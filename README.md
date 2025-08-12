# CShorelark 🦅

A modern C++17 port of the fascinating [Shorelark](https://github.com/patryk27/shorelark) project - a bird evolution simulator that demonstrates genetic algorithms and neural networks in action.

[![Build & Test](https://github.com/karnotxo/cshorelark/actions/workflows/build.yml/badge.svg)](https://github.com/karnotxo/cshorelark/actions/workflows/build.yml)
[![Code Quality](https://github.com/karnotxo/cshorelark/actions/workflows/code-quality.yml/badge.svg)](https://github.com/karnotxo/cshorelark/actions/workflows/code-quality.yml)
[![Documentation](https://github.com/karnotxo/cshorelark/actions/workflows/documentation.yml/badge.svg)](https://github.com/karnotxo/cshorelark/actions/workflows/documentation.yml)
[![Security](https://github.com/karnotxo/cshorelark/actions/workflows/security.yml/badge.svg)](https://github.com/karnotxo/cshorelark/actions/workflows/security.yml)
[![codecov](https://codecov.io/gh/karnotxo/cshorelark/branch/main/graph/badge.svg)](https://codecov.io/gh/karnotxo/cshorelark)
[![Documentation Status](https://img.shields.io/github/deployments/karnotxo/cshorelark/github-pages?label=docs)](https://karnotxo.github.io/cshorelark/)
[![License](https://img.shields.io/github/license/karnotxo/cshorelark)](LICENSE)
[![C++17](https://img.shields.io/badge/C%2B%2B-17-blue.svg)](https://en.wikipedia.org/wiki/C%2B%2B17)
[![Conan](https://img.shields.io/badge/package%20manager-conan-pink.svg)](https://conan.io/)
[![CMake](https://img.shields.io/badge/CMake-3.20%2B-green.svg)](https://cmake.org/)
[![Meson](https://img.shields.io/badge/Meson-1.0%2B-orange.svg)](https://mesonbuild.com/)

## Overview

CShorelark reimagines the original Rust/TypeScript project in modern C++, bringing the excitement of artificial life and evolution to native performance. Watch as birds learn to navigate through obstacles using neural networks and genetic algorithms, evolving better strategies with each generation.

## Original Project

This project is based on the excellent work by [Patryk Wychowaniec](https://github.com/patryk27) and his detailed blog series:

1. [Learning to Fly - Part 1: Genetic Algorithms](https://pwy.io/posts/learning-to-fly-pt1/)
2. [Learning to Fly - Part 2: Neural Networks](https://pwy.io/posts/learning-to-fly-pt2/)
3. [Learning to Fly - Part 3: Backpropagation](https://pwy.io/posts/learning-to-fly-pt3/)
4. [Learning to Fly - Part 4: Putting it All Together](https://pwy.io/posts/learning-to-fly-pt4/)

## How It Works

CShorelark combines several key components:

### 🧬 Genetic Algorithm

- Manages a population of birds
- Uses natural selection to evolve better flying strategies
- Implements crossover and mutation operations to create new generations

### 🧠 Neural Network

- Each bird has its own neural network "brain"
- Processes inputs like position, velocity, and obstacle locations
- Determines flight adjustments in real-time

### 🎮 Dear ImGui Visualization

- Real-time visualization of the evolution process
- Watch birds learn and adapt over generations
- Interactive controls for simulation parameters

## Key Features

- Modern C++17 implementation with C++20 features via compatibility libraries
- Memory-safe design using RAII and smart pointers
- High-performance native code
- Dear ImGui-based user interface
- Real-time evolution visualization
- Configurable simulation parameters
- Educational tool for understanding AI concepts

## Requirements

- C++17 compatible compiler (GCC 9+, Clang 10+, MSVC 2019+)
- CMake 3.20+ or Meson 1.0.0+
- Conan 2.0+ package manager

## Dependencies

All dependencies are managed through Conan:

- Dear ImGui (GUI)
- {fmt} (Modern formatting)
- spdlog (Logging)
- range-v3 (Ranges library)
- span-lite (span implementation)
- tl::expected (Error handling)
- tl::optional (Optional type)
- Catch2 (Testing)
- GLFW/GLEW (OpenGL)
- transwarp (Task graphs)
- tomlplusplus (Configuration)

## Build Options & Compiler Flags

### Common Compiler Flags

```bash
# GCC/Clang
-Wall -Wextra -Wpedantic    # Essential warnings
-Werror                     # Treat warnings as errors
-fno-rtti                   # Disable RTTI for better performance
-march=native              # Optimize for local machine
-O3                        # Aggressive optimization

# MSVC
/W4                        # High warning level
/WX                        # Treat warnings as errors
/O2                        # Optimize for speed
/GL                        # Whole program optimization
/LTCG                      # Link-time code generation
```

### CMake Build Options

```bash
# Debug build with sanitizers
cmake --preset conan-debug -DENABLE_SANITIZERS=ON -DENABLE_TESTING=ON

# Release build with benchmarks
cmake --preset conan-release -DENABLE_BENCHMARKS=ON -DUSE_LTO=ON

# Profile guided optimization
cmake --preset conan-release -DENABLE_PGO=ON
```

### Meson Build Options

```bash
# Debug build with sanitizers
meson setup builddir --pkg-config-path=build -Dbuildtype=debug -Db_sanitize=address,undefined

# Release build with LTO
meson setup builddir --pkg-config-path=build -Dbuildtype=release -Db_lto=true

# Custom optimization level
meson setup builddir --pkg-config-path=build -Doptimization=3
```

## Simulation Parameters

### World Configuration

| Parameter | Description | Default | Range |
|-----------|-------------|---------|--------|
| `world_width` | Simulation world width | 1600 | 800-3200 |
| `world_height` | Simulation world height | 900 | 600-1800 |
| `food_count` | Number of food particles | 40 | 10-100 |
| `food_spawn_interval` | Ticks between food spawns | 100 | 50-500 |

### Bird Parameters

| Parameter | Description | Default | Range |
|-----------|-------------|---------|--------|
| `population_size` | Number of birds | 50 | 10-200 |
| `bird_size` | Bird radius | 8.0 | 4.0-16.0 |
| `max_speed` | Maximum bird velocity | 15.0 | 5.0-30.0 |
| `vision_range` | Bird's vision distance | 200.0 | 50.0-400.0 |
| `vision_angle` | Bird's field of view | 120° | 60°-180° |

### Neural Network Configuration

| Parameter | Description | Default | Range |
|-----------|-------------|---------|--------|
| `hidden_layers` | Number of hidden layers | 2 | 1-4 |
| `neurons_per_layer` | Neurons in hidden layers | 8 | 4-16 |
| `activation_function` | Neural activation | ReLU | ReLU/Tanh/Sigmoid |
| `input_neurons` | Input layer size | 5 | Fixed |
| `output_neurons` | Output layer size | 2 | Fixed |

### Genetic Algorithm Settings

| Parameter | Description | Default | Range |
|-----------|-------------|---------|--------|
| `mutation_rate` | Gene mutation probability | 0.05 | 0.01-0.20 |
| `crossover_rate` | Breeding probability | 0.70 | 0.50-0.90 |
| `elite_count` | Birds preserved as-is | 5 | 1-10 |
| `tournament_size` | Selection tournament size | 3 | 2-7 |

### Performance Settings

| Parameter | Description | Default | Range |
|-----------|-------------|---------|--------|
| `target_fps` | Target frame rate | 60 | 30-144 |
| `simulation_speed` | Simulation multiplier | 1.0 | 0.1-10.0 |
| `thread_count` | Worker threads | Auto | 1-32 |
| `batch_size` | Neural network batch size | 32 | 8-128 |

## Building and Running

### Using CMake

```bash
# Clone the repository
git clone https://github.com/yourusername/cshorelark.git
cd cshorelark

# Install dependencies with Conan
conan install . --output-folder=build --build=missing

# Configure and build
cmake --preset conan-release
cmake --build --preset conan-release

# Run the application
./build/Release/bin/simulation-ui
```

### Using Meson

```bash
# Clone the repository
git clone https://github.com/yourusername/cshorelark.git
cd cshorelark

# Install dependencies with Conan
conan install . --output-folder=build --build=missing

# Configure and build
meson setup builddir --pkg-config-path=build
meson compile -C builddir

# Run the application
./builddir/apps/simulation-ui/simulation-ui
```

## Project Structure

```
cshorelark/
├── apps/
│   ├── simulation-ui/    # GUI application
│   └── optimizer-cli/    # Command-line optimizer
├── libs/
│   ├── genetic-algorithm/# Genetic algorithm implementation
│   ├── neural-network/   # Neural network implementation
│   ├── optimizer/        # Training optimizer
│   └── simulation/       # Core simulation logic
├── include/             # Public headers
├── docs/               # Documentation
└── tests/              # Test suite
```

## Documentation

Documentation is automatically generated during the build process using a combination of Doxygen and Sphinx. The generated documentation includes:

- API Reference (Doxygen)
- Architecture Guide
- User Manual
- Developer Guide
- Examples & Tutorials

### Documentation Structure

```
docs/
├── api/              # API documentation (Doxygen)
├── sphinx/           # Comprehensive documentation (Sphinx)
│   ├── architecture/ # Architecture documentation
│   ├── examples/     # Code examples and tutorials
│   ├── manual/       # User manual
│   └── dev-guide/    # Developer documentation
└── assets/          # Documentation assets
```

### Building Documentation

#### Using CMake

```bash
# Build documentation only
cmake --build build --target docs

# Build as part of main build
cmake --preset conan-release -DBUILD_DOCS=ON
cmake --build build
```

#### Using Meson

```bash
# Configure with documentation enabled
meson setup builddir -Dbuild_docs=true

# Build documentation
meson compile docs -C builddir
```

### Documentation Tools

- **Doxygen**: API documentation from source code
- **Sphinx**: Main documentation framework
- **Breathe**: Doxygen-Sphinx bridge
- **sphinx-rtd-theme**: Documentation theme
- **sphinxcontrib-mermaid**: Diagrams and charts
- **sphinx-multiversion**: Version-specific docs

### Writing Documentation

#### Code Documentation

```cpp
/// @brief Neural network implementation
/// @details Implements a feedforward neural network with configurable layers
class NeuralNetwork {
public:
    /// @brief Constructs a neural network
    /// @param layers Vector of layer sizes
    /// @param activation Activation function to use
    NeuralNetwork(std::vector<size_t> layers,
                 ActivationFunction activation = ActivationFunction::ReLU);
    
    // ... rest of the class
};
```

#### Sphinx Documentation

```rst
Neural Network Architecture
==========================

.. mermaid::

   graph TD
       A[Input Layer] --> B[Hidden Layer 1]
       B --> C[Hidden Layer 2]
       C --> D[Output Layer]
```

### Continuous Documentation

Documentation is automatically:

- Generated on each release
- Deployed to GitHub Pages
- Version-controlled alongside code
- Checked for broken links and syntax
- Cross-referenced with code

## Contributing

Contributions are welcome! Feel free to:

- Report bugs
- Suggest features
- Submit pull requests

Please ensure your code follows modern C++ best practices and includes appropriate tests.

## License

This project is licensed under the same terms as the original Shorelark project.

## Acknowledgments

Special thanks to Patryk Wychowaniec for creating the original Shorelark project and writing the excellent blog series that made this port possible.
