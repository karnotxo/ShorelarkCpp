#ifndef CSHORELARK_NEURAL_NETWORK_LAYER_H
#define CSHORELARK_NEURAL_NETWORK_LAYER_H

/**
 * @file layer.h
 * @brief Neural network layer implementation
 */

// C++ system headers
#include <memory>
#include <vector>

// External library headers
#include <nonstd/span.hpp>
#include <tl/expected.hpp>

// Project headers
#include "neural_network/activation.h"  // NOLINT
#include "neural_network/neuron.h"      // NOLINT
#include "random/random.h"              // NOLINT

namespace cshorelark::neural_network {

/**
 * @brief Error types that can occur during layer operations
 */
enum class layer_error {
    k_invalid_input_size,  ///< Input size doesn't match layer topology
    k_not_enough_weights,  ///< Fewer weights provided than needed
    k_empty_neurons,       ///< No neurons provided
    k_mismatched_inputs    ///< Neurons have different input sizes
};

/**
 * @brief A layer of neurons in a neural network
 *
 * The layer class represents a collection of neurons that form
 * a single layer in a feed-forward neural network.
 *
 * @tparam T Value type for neuron computations (default: float)
 */
template <typename T>
class layer {
public:
    // Type aliases
    using value_type = T;
    using neuron_type = neuron<T>;

    /**
     * @brief Constructs a layer with specified neurons
     * @param neurons Vector of neurons making up this layer
     * @return Expected containing layer or error if validation fails
     */
    static auto create(std::vector<neuron_type> neurons) -> tl::expected<layer<T>, layer_error>;

    /**
     * @brief Copy constructor for layer is deleted since neurons cannot be copied
     */
    layer(const layer& other) = delete;

    /**
     * @brief Copy assignment operator for layer is deleted since neurons cannot be copied
     */
    layer& operator=(const layer& other) = delete;

    // Allow moving
    layer(layer&&) noexcept = default;
    layer& operator=(layer&&) noexcept = default;

    /**
     * @brief Gets the number of inputs this layer accepts
     */
    [[nodiscard]] std::size_t input_size() const {
        return neurons_.empty() ? 0 : neurons_.front().input_size();
    }

    /**
     * @brief Gets the number of neurons in this layer
     */
    [[nodiscard]] std::size_t size() const { return neurons_.size(); }

    /**
     * @brief Gets the total number of weights in all neurons
     */
    [[nodiscard]] std::size_t weight_count() const {
        return neurons_.empty() ? 0 : (neurons_.front().input_size() + 1) * neurons_.size();
    }

    /**
     * @brief Creates a layer from a range of weights
     * @param input_size Number of inputs each neuron should accept
     * @param output_size Number of neurons to create
     * @param weights Span to the weights
     * @return A new layer or an error if not enough weights
     */
    static auto from_weights(std::size_t input_size, std::size_t output_size,
                             nonstd::span<const T> weights) -> tl::expected<layer<T>, layer_error>;

    /**
     * @brief Creates a layer with random weights
     * @param random Random number generator to use
     * @param input_size Number of inputs for each neuron
     * @param output_size Number of neurons in the layer
     * @return A new layer with random weights
     */
    static auto random(cshorelark::random::random_generator& random, std::size_t input_size,
                       std::size_t output_size) -> tl::expected<layer<T>, layer_error>;

    /**
     * @brief Processes inputs through the layer
     * @param inputs Span of input values
     * @return Expected containing output values or error if processing fails
     */
    [[nodiscard]] tl::expected<std::vector<value_type>, layer_error> propagate(
        nonstd::span<const value_type> inputs) const;

    /**
     * @brief Gets all weights in the layer as a flat vector
     * @return Vector containing all weights
     */
    [[nodiscard]] auto weights() const -> std::vector<value_type>;

    /**
     * @brief Gets the neurons in this layer
     * @return Const reference to the neurons vector
     */
    [[nodiscard]] auto get_neurons() const -> const std::vector<neuron_type>& { return neurons_; }

private:
    explicit layer(std::vector<neuron_type> neurons) : neurons_(std::move(neurons)) {}
    layer(cshorelark::random::random_generator& random, std::size_t input_size,
          std::size_t output_size);
    std::vector<neuron_type> neurons_;  ///< Neurons in this layer
};

}  // namespace cshorelark::neural_network

#endif  // CSHORELARK_NEURAL_NETWORK_LAYER_H_