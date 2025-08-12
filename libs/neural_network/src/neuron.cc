#include "neural_network/neuron.h"  // NOLINT

#include <algorithm>
#include <cmath>  // For std::max
#include <cstddef>
#include <nonstd/span.hpp>
#include <numeric>
#include <tl/expected.hpp>
#include <vector>

#include "random/random.h"  // NOLINT

namespace cshorelark::neural_network {

// Implementation of the weights method that returns all weights including bias
template <typename T>
auto neuron<T>::to_weights() const -> std::vector<T> {
    // Return bias first, then weights (matches Rust chain of once(bias).chain(weights))
    std::vector<T> result;
    result.reserve(weights_.size() + 1);
    result.push_back(bias_);
    result.insert(result.end(), weights_.begin(), weights_.end());
    return result;
}

template <typename T>
auto neuron<T>::random(cshorelark::random::random_generator& random,
                       std::size_t input_size) -> neuron<T> {
    // Create random weights for the neuron
    std::vector<value_type> weights;
    weights.reserve(input_size);

    for (std::size_t i = 0; i < input_size; ++i) {
        weights.push_back(random.generate_weight());
    }

    // Random bias
    T bias = random.generate_weight();

    // Return a new neuron using move semantics
    return neuron<T>(bias, std::move(weights));
}

// Implementation of the process method
template <typename T>
auto neuron<T>::propagate(nonstd::span<const T> inputs) const -> tl::expected<T, neuron_error> {
    if (inputs.size() != weights_.size()) {
        return tl::make_unexpected(neuron_error::k_invalid_input_size);
    }

    // Calculate weighted sum(weights * inputs) + bias
    const value_type sum =
        std::inner_product(inputs.begin(), inputs.end(), weights_.begin(), bias_);

    return std::max(static_cast<T>(0.0F), sum);  // Apply activation function (ReLU)
}

// Implementation of from_weights template method
template <typename T>
auto neuron<T>::from_weights(std::size_t input_size, nonstd::span<const T> weights)
    -> tl::expected<neuron<T>, neuron_error> {
    // Check if we have exactly the right number of weights
    if (weights.size() != input_size + 1) {  // +1 for bias
        return tl::make_unexpected(neuron_error::k_invalid_weights_size);
    }

    // First weight is the bias
    T bias = weights[0];

    // Remaining weights are for connections
    std::vector<T> connection_weights(weights.begin() + 1, weights.end());

    return neuron{bias, std::move(connection_weights)};
}

// Explicitly instantiate the template for float
template class neuron<float>;

}  // namespace cshorelark::neural_network
