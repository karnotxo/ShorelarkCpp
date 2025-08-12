#include "neural_network/layer.h"  // NOLINT

// C++ system headers
#include <numeric>
#include <vector>

// External library headers
#include <range/v3/algorithm/transform.hpp>
#include <range/v3/view/join.hpp>

#include "random/random.h"

namespace cshorelark::neural_network {

template <typename T>
layer<T>::layer(cshorelark::random::random_generator& random, std::size_t input_size,
                std::size_t output_size) {
    neurons_.reserve(output_size);
    for (std::size_t i = 0; i < output_size; ++i) {
        // Use the static random method instead of trying to construct directly
        neurons_.push_back(neuron<T>::random(random, input_size));
    }
}
template <typename T>
tl::expected<layer<T>, layer_error> layer<T>::create(std::vector<neuron_type> neurons) {
    // Match Rust implementation's assertions
    if (neurons.empty()) {
        return tl::make_unexpected(layer_error::k_empty_neurons);
    }

    const auto first_size = neurons.front().input_size();
    if (!std::all_of(neurons.begin(), neurons.end(),
                     [first_size](const auto& n) { return n.input_size() == first_size; })) {
        return tl::make_unexpected(layer_error::k_mismatched_inputs);
    }

    return layer{std::move(neurons)};
}

template <typename T>
auto layer<T>::random(cshorelark::random::random_generator& random, std::size_t input_size,
                      std::size_t output_size) -> tl::expected<layer<T>, layer_error> {
    if (input_size == 0) {
        return tl::make_unexpected(layer_error::k_invalid_input_size);
    }

    return layer<T>{random, input_size, output_size};
}

// Template implementation for layer::process
template <typename T>
auto layer<T>::propagate(nonstd::span<const value_type> inputs) const
    -> tl::expected<std::vector<typename layer<T>::value_type>, layer_error> {
    if (inputs.size() != input_size()) {
        return tl::make_unexpected(layer_error::k_invalid_input_size);
    }

    std::vector<value_type> outputs;
    outputs.reserve(size());

    for (const auto& neuron : neurons_) {
        auto result = neuron.propagate(inputs);
        if (!result) {
            return tl::make_unexpected(layer_error::k_invalid_input_size);
        }
        outputs.push_back(*result);
    }

    return outputs;
}

template <typename T>
auto layer<T>::weights() const -> std::vector<value_type> {
    std::vector<value_type> weights;
    weights.reserve(weight_count());

    for (const auto& neuron : neurons_) {
        auto neuron_weights = neuron.to_weights();
        weights.insert(weights.end(), neuron_weights.begin(), neuron_weights.end());
    }

    return weights;
}

// Template implementation for from_weights
template <typename T>
auto layer<T>::from_weights(std::size_t input_size, std::size_t output_size,
                            nonstd::span<const T> weights) -> tl::expected<layer<T>, layer_error> {
    std::vector<typename layer<T>::neuron_type> neurons;
    neurons.reserve(output_size);

    // Calculate weights needed per neuron (inputs + bias)
    const std::size_t weights_per_neuron = input_size + 1;

    // Check if there are enough weights
    if (weights.size() < weights_per_neuron * output_size) {
        return tl::make_unexpected(layer_error::k_not_enough_weights);
    }

    std::size_t current_offset = 0;
    for (std::size_t i = 0; i < output_size; ++i) {
        // Create a subspan for the current neuron's weights
        auto neuron_weights = weights.subspan(current_offset, weights_per_neuron);

        // Use the neuron::from_weights method with the subspan
        auto neuron_result = neuron<T>::from_weights(input_size, neuron_weights);

        if (!neuron_result) {
            return tl::make_unexpected(layer_error::k_not_enough_weights);
        }

        neurons.push_back(std::move(*neuron_result));

        // Advance the offset by the number of weights used for this neuron
        current_offset += weights_per_neuron;
    }

    return layer<T>::create(std::move(neurons));
}

// Explicit instantiation of template methods for float type
template class layer<float>;
template auto layer<float>::from_weights(std::size_t input_size, std::size_t output_size,
                                         nonstd::span<const float> weights)
    -> tl::expected<layer<float>, layer_error>;

}  // namespace cshorelark::neural_network