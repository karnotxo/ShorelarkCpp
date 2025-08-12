#include "neural_network/network.h"

#include <algorithm>
#include <cstdio>
#include <memory>
#include <numeric>
#include <range/v3/algorithm/transform.hpp>
#include <range/v3/view/join.hpp>
#include <stdexcept>
#include <tl/expected.hpp>
#include <vector>

#include "neural_network/layer.h"
#include "neural_network/layer_topology.h"
#include "random/random.h"

namespace cshorelark::neural_network {

// Explicitly instantiate from_weights with std::vector iterators
template <typename T>
auto network<T>::from_weights(nonstd::span<const neural_network::layer_topology> topology,
                              nonstd::span<const T> weights)
    -> tl::expected<network<T>, network_error> {
    if (topology.size() < 2) {
        return tl::make_unexpected(network_error::k_invalid_layer_count);
    }

    std::vector<layer_type> layers;
    layers.reserve(topology.size() - 1);

    std::size_t current_offset = 0;
    for (std::size_t i = 0; i < topology.size() - 1; ++i) {
        const auto weights_per_layer = (topology[i].neurons() + 1) * topology[i + 1].neurons();
        auto layer_weights = weights.subspan(current_offset, weights_per_layer);
        auto layer_result = layer_type::from_weights(topology[i].neurons(),
                                                     topology[i + 1].neurons(), layer_weights);
        if (!layer_result) {
            switch (layer_result.error()) {
                case layer_error::k_not_enough_weights:
                    return tl::make_unexpected(network_error::k_not_enough_weights);
                case layer_error::k_empty_neurons:
                case layer_error::k_mismatched_inputs:
                case layer_error::k_invalid_input_size:
                    return tl::make_unexpected(network_error::k_invalid_layer_topology);
            }
        }
        layers.push_back(std::move(*layer_result));
        // Advance the iterator by the number of weights used in this layer
        current_offset += weights_per_layer;
    }

    if (current_offset != weights.size()) {
        return tl::make_unexpected(network_error::k_too_many_weights);
    }

    return network(std::move(layers));
}

template <typename T>
auto network<T>::random(nonstd::span<const neural_network::layer_topology> topology,
                        cshorelark::random::random_generator& random_gen)
    -> tl::expected<network<T>, network_error> {
    if (topology.size() < 2) {
        return tl::make_unexpected(network_error::k_invalid_layer_count);
    }

    std::vector<layer_type> layers;
    layers.reserve(topology.size() - 1);

    // Create layers between each pair of adjacent topologies
    for (std::size_t i = 0; i < topology.size() - 1; ++i) {
        auto layer_result =
            layer_type::random(random_gen, topology[i].neurons(), topology[i + 1].neurons());
        if (!layer_result) {
            return tl::make_unexpected(network_error::k_invalid_layer_topology);
        }
        layers.push_back(std::move(*layer_result));
    }

    return network(std::move(layers));
}

template <typename T>
auto network<T>::propagate(nonstd::span<const value_type> inputs) const
    -> tl::expected<std::vector<value_type>, network_error> {
    // Verify we have at least one layer
    if (layers_.empty()) {
        return tl::make_unexpected(network_error::k_network_not_initialized);
    }

    // Check if input size matches what the first layer expects
    if (inputs.size() != input_size()) {
        return tl::make_unexpected(network_error::k_invalid_input_size);
    }

    // Process through all layers, propagating outputs of each to the next
    std::vector<value_type> outputs = std::vector<value_type>(inputs.begin(), inputs.end());

    for (const auto& layer : layers_) {
        // Feed outputs from previous layer as inputs to current layer
        auto result = layer.propagate(outputs);
        if (!result) {
            // Map layer error to network error
            return tl::make_unexpected(network_error::k_propagation_error);
        }
        outputs = std::move(*result);
    }

    return outputs;
}

template <typename T>
auto network<T>::weights() const -> std::vector<T> {
    // Implementation similar to Rust:
    // pub fn weights(&self) -> impl Iterator<Item = f32> + '_ {
    //     self.layers
    //         .iter()
    //         .flat_map(|layer| layer.neurons.iter())
    //         .flat_map(|neuron| once(&neuron.bias).chain(&neuron.weights))
    //         .cloned()
    // }

    // Calculate total weights count for pre-allocation
    std::size_t total_weights = 0;
    for (const auto& layer : layers_) {
        total_weights += layer.weight_count();
    }

    std::vector<T> all_weights;
    all_weights.reserve(total_weights);

    // Flat map through layers -> neurons -> (bias + weights)
    for (const auto& layer : layers_) {
        for (const auto& neuron : layer.get_neurons()) {
            // Get neuron weights including bias as first element
            auto neuron_weights_full = neuron.to_weights();
            all_weights.insert(all_weights.end(), neuron_weights_full.begin(),
                               neuron_weights_full.end());
        }
    }

    return all_weights;
}

template <typename T>
auto network<T>::input_size() const -> std::size_t {
    return layers_.empty() ? 0 : layers_.front().input_size();
}

template <typename T>
auto network<T>::output_size() const -> std::size_t {
    return layers_.empty() ? 0 : layers_.back().size();
}

/**
 * @brief Gets the total number of weights in the network
 * @return Total weight count
 */
template <typename T>
[[nodiscard]] auto network<T>::weight_count() const -> std::size_t {
    std::size_t count = 0;
    for (const auto& layer : layers_) {
        count += layer.weight_count();
    }
    return count;
}

// Explicit template instantiation for the float type used in simulation
template class network<float>;

}  // namespace cshorelark::neural_network
