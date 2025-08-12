#ifndef CSHORELARK_NEURAL_NETWORK_NEURON_H
#define CSHORELARK_NEURAL_NETWORK_NEURON_H

/**
 * @file neuron.h
 * @brief Represents a single neuron in a neural network
 */

// C++ system headers
#include <algorithm>
#include <nonstd/span.hpp>
#include <tl/expected.hpp>
#include <vector>

// Project headers
#include "random/random.h"

namespace cshorelark::neural_network {

/**
 * @brief Error types that can occur during neuron operations
 */
enum class neuron_error {
    k_invalid_input_size,   ///< Input size doesn't match neuron's expected input count
    k_not_enough_weights,   ///< Not enough weights provided for neuron construction
    k_invalid_weights_size  ///< Invalid number of weights provided
};

/**
 * @brief A single neuron in a neural network
 */
template <typename T = float>
class neuron {
public:
    using value_type = T;

    /**
     * @brief Creates a neuron with specified weights and bias
     *
     * @param bias Bias value applied to weighted sum
     * @param weights Vector of weights for each input connection
     */
    neuron(T bias, std::vector<T> weights) : bias_(bias), weights_(std::move(weights)) {}

    /**
     * @brief Move constructor for neuron
     *
     * @param other Neuron to move from
     */
    neuron(neuron&& other) noexcept : bias_(other.bias_), weights_(std::move(other.weights_)) {}

    /**
     * @brief Move assignment operator for neuron
     *
     * @param other Neuron to move from
     * @return Reference to this neuron
     */
    neuron& operator=(neuron&& other) noexcept {
        if (this != &other) {
            bias_ = other.bias_;
            weights_ = std::move(other.weights_);
        }
        return *this;
    }

    // Delete copy constructor and copy assignment operator to prevent copying
    neuron(const neuron&) = delete;
    neuron& operator=(const neuron&) = delete;

    /**
     * @brief Creates a random neuron with the specified number of inputs
     * @param input_size Number of inputs for the neuron
     * @param random Random number generator to use
     * @return A new neuron with random weights
     */
    [[nodiscard]] static auto random(cshorelark::random::random_generator& random,
                                     std::size_t input_size) -> neuron<T>;

    /**
     * @brief Gets the number of inputs this neuron accepts
     * @return Input count
     */
    [[nodiscard]] std::size_t input_size() const noexcept { return weights_.size(); }

    /**
     * @brief Gets the raw weights vector (without bias) for this neuron
     * @return Const reference to the internal weights vector
     */
    [[nodiscard]] const std::vector<T>& weights() const noexcept { return weights_; }

    /**
     * @brief Gets all weights including bias as a flat vector
     * @return Vector containing all weights and bias
     */
    [[nodiscard]] std::vector<T> to_weights() const;

    /**
     * @brief Gets the bias for this neuron
     * @return Bias value
     */
    [[nodiscard]] auto bias() const noexcept -> T { return bias_; }

    /**
     * @brief Sets the weights for this neuron
     * @param weights Vector of new weights
     */
    void set_weights(std::vector<T> weights) { weights_ = std::move(weights); }

    /**
     * @brief Sets the bias for this neuron
     * @param bias New bias value
     */
    void set_bias(T bias) { bias_ = bias; }

    /**
     * @brief Propagates input values through this neuron
     * @param inputs Span of input values
     * @return Expected containing output value or error if propagation fails
     */
    [[nodiscard]] auto propagate(nonstd::span<const T> inputs) const
        -> tl::expected<T, neuron_error>;

    /**
     * @brief Creates a neuron with weights from a span
     * @param input_size Number of inputs this neuron accepts
     * @param weights Span containing weights (with bias as first element)
     * @return Expected containing neuron or error if invalid
     */
    [[nodiscard]] static auto from_weights(std::size_t input_size, nonstd::span<const T> weights)
        -> tl::expected<neuron, neuron_error>;

private:
    T bias_;                  ///< Bias value
    std::vector<T> weights_;  ///< Weights for each input connection
};

}  // namespace cshorelark::neural_network

#endif  // CSHORELARK_NEURAL_NETWORK_NEURON_H