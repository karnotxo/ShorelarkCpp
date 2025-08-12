#ifndef CSHORELARK_NEURAL_NETWORK_NETWORK_H
#define CSHORELARK_NEURAL_NETWORK_NETWORK_H

/**
 * @file network.h
 * @brief Neural network implementation supporting feed-forward propagation
 */

// C++ system headers
#include <array>
#include <cstddef>
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

// External library headers
#include <spdlog/spdlog.h>

#include <nonstd/span.hpp>
#include <tl/expected.hpp>

// Project headers
#include "neural_network/layer.h"           // NOLINT
#include "neural_network/layer_topology.h"  // NOLINT
#include "random/random.h"                  // NOLINT

namespace cshorelark::neural_network {

/**
 * @brief Error types that can occur during network operations
 */
enum class network_error {
    k_invalid_layer_count,      ///< Network must have at least 2 layers
    k_invalid_input_size,       ///< Input size doesn't match network topology
    k_too_many_weights,         ///< More weights provided than needed
    k_not_enough_weights,       ///< Fewer weights provided than needed
    k_network_not_initialized,  ///< Network not initialized
    k_propagation_error,        ///< Propagation error
    k_invalid_layer_topology    ///< Layer topology is invalid (empty or mismatched inputs)
};

/**
 * @brief A feed-forward neural network composed of multiple layers
 *
 * @tparam T Value type for network computations (default: float)
 */
template <typename T = float>  // Add the default parameter here to match the forward declaration
class network {
public:
    // Type aliases
    using value_type = T;
    using reference = value_type&;
    using const_reference = const value_type&;
    using layer_type = layer<T>;

    /**
     * @brief Constructs a network from a vector of layers
     * @param layers Vector of layers making up the network
     */
    explicit network(std::vector<layer_type> layers) : layers_(std::move(layers)) {
    }

    /**
     * @brief Copy constructor is deleted since layers cannot be copied
     */
    network(const network& other) = delete;

    /**
     * @brief Copy assignment operator is deleted since layers cannot be copied
     */
    network& operator=(const network& other) = delete;

    /**
     * @brief Moving is allowed and defaulted
     */
    network(network&&) noexcept = default;

    /**
     * @brief Moving assignment is allowed and defaulted
     */
    network& operator=(network&&) noexcept = default;

    /**
     * @brief Gets the number of inputs this network accepts
     * @return Number of inputs
     */
    [[nodiscard]] auto input_size() const -> std::size_t;

    /**
     * @brief Gets the number of outputs this network produces
     * @return Number of outputs
     */
    [[nodiscard]] auto output_size() const -> std::size_t;

    /**
     * @brief Gets the total number of weights in the network
     * @return Total weight count
     */
    [[nodiscard]] auto weight_count() const -> std::size_t;

    /**
     * @brief Creates a network from layer topologies and a span of weights
     * @param topology Topologies of each layer in the network
     * @param weights Span of weights to initialize the network
     * @return Expected containing network or error
     */
    [[nodiscard]] static auto from_weights(
        nonstd::span<const neural_network::layer_topology> topology,
        nonstd::span<const value_type> weights) -> tl::expected<network<T>, network_error>;

    /**
     * @brief Creates a network with random weights
     * @param topology Vector of layer topologies defining the network structure
     * @param random_gen Random number generator to use
     * @return Expected containing network or error if creation fails
     */
    static auto random(nonstd::span<const neural_network::layer_topology> topology,
                       cshorelark::random::random_generator& random_gen)
        -> tl::expected<network<T>, network_error>;

    /**
     * @brief Propagates input values through the network
     * @param inputs Span of input values
     * @return Expected containing output values or error if propagation fails
     */
    [[nodiscard]] auto propagate(nonstd::span<const value_type> inputs) const
        -> tl::expected<std::vector<value_type>, network_error>;

    /**
     * @brief Gets all weights in the network as a flat vector
     * @return Vector containing all weights
     */
    [[nodiscard]] auto weights() const -> std::vector<value_type>;

private:
    std::vector<layer_type> layers_;  ///< Layers in the network
};

// Type alias for a standard network implementation
// This matches the Rust implementation's approach
using standard_network = network<float>;
}  // namespace cshorelark::neural_network

#endif  // CSHORELARK_NEURAL_NETWORK_NETWORK_H_
