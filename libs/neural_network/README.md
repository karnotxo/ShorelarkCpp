# Neural Network Library

A modern C++17 implementation of a feed-forward neural network, designed for use in the Shorelark project. This library provides a clean, safe, and efficient neural network implementation with strong type safety and modern C++ features.

## Features

- Modern C++17 implementation with C++20 features via compatibility libraries
- Header-only library for easy integration
- Strong type safety and RAII design
- Exception-safe error handling using `tl::expected`
- Efficient memory management with move semantics
- Comprehensive test coverage using Catch2
- Full documentation with Doxygen

## Requirements

- C++17 compliant compiler
- CMake 3.20 or higher
- Dependencies (automatically handled by Conan):
  - range-v3
  - span-lite
  - tl-expected
  - tl-optional
  - fmt
  - spdlog
  - Catch2 (for testing)

## Usage

### Basic Example

```cpp
#include <neural_network/network.h>
#include <random>

using namespace shorelark;

// Create a 3-2-1 network topology
std::vector<LayerTopology> topology = {
    LayerTopology{3}, LayerTopology{2}, LayerTopology{1}
};

// Create random network
std::mt19937 rng{std::random_device{}()};
auto network = Network::random(rng, topology);

// Propagate inputs
std::vector<double> inputs = {0.5, 0.6, 0.7};
auto outputs = network->propagate(inputs);

if (outputs) {
    // Use the outputs
    double result = (*outputs)[0];
} else {
    // Handle error
    switch (outputs.error()) {
        case NetworkError::InvalidInputSize:
            // Handle invalid input size
            break;
        // Handle other errors...
    }
}
```

### Creating a Network from Weights

```cpp
std::vector<LayerTopology> topology = {
    LayerTopology{2}, LayerTopology{3}, LayerTopology{1}
};

std::vector<double> weights = {
    // First layer (2->3): 6 weights + 3 biases
    0.1, 0.2, 0.3,  // Biases
    0.4, 0.5, 0.6, 0.7, 0.8, 0.9,  // Weights
    
    // Second layer (3->1): 3 weights + 1 bias
    0.1,  // Bias
    0.2, 0.3, 0.4  // Weights
};

auto network = Network::from_weights(topology, weights.begin(), weights.end());
```

## Architecture

The library is composed of three main components:

1. **Neuron**: The basic processing unit that applies weights to inputs and an activation function
2. **Layer**: A collection of neurons that process inputs in parallel
3. **Network**: The complete neural network that manages layers and provides the main interface

### Key Design Decisions

- Use of `tl::expected` for error handling instead of exceptions
- Zero-copy views with `nonstd::span` for input/output operations
- Modern ranges with range-v3 for efficient data processing
- Header-only design for easy integration and optimization
- Strong type safety with compile-time checks where possible

## Building and Testing

```bash
# Configure with tests enabled
cmake -B build -S . -DBUILD_TESTING=ON

# Build
cmake --build build

# Run tests
ctest --test-dir build

# Generate documentation (requires Doxygen)
cmake --build build --target neural_network_docs
```

## Contributing

When contributing to this library, please:

1. Follow the modern C++ guidelines
2. Maintain const-correctness and RAII principles
3. Add tests for new functionality
4. Update documentation for public APIs
5. Use the provided sanitizers in debug builds

## License

This library is part of the Shorelark project and is licensed under the same terms.
