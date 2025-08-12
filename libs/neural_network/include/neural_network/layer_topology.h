#ifndef CSHORELARK_NEURAL_NETWORK_LAYER_TOPOLOGY_H
#define CSHORELARK_NEURAL_NETWORK_LAYER_TOPOLOGY_H

/**
 * @file layer_topology.h
 * @brief Definition of neural network layer topology
 */

#include <cstddef>

namespace cshorelark::neural_network {

/**
 * @brief Describes the topology of a neural network layer
 *
 * This class is used to specify the number of neurons in each layer
 * when constructing a neural network.
 */
class layer_topology {
public:
    /**
     * @brief Constructs a layer topology with specified neuron count
     * @param neurons Number of neurons in this layer
     */
    explicit layer_topology(std::size_t neurons) noexcept : neurons_(neurons) {}

    /**
     * @brief Gets the number of neurons in this layer
     */
    [[nodiscard]] auto neurons() const noexcept -> std::size_t { return neurons_; }

private:
    std::size_t neurons_;  ///< Number of neurons in this layer
};

}  // namespace cshorelark::neural_network

#endif  // CSHORELARK_NEURAL_NETWORK_LAYER_TOPOLOGY_H_