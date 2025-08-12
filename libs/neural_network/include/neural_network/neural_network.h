#ifndef NEURAL_NETWORK_NEURAL_NETWORK_H
#define NEURAL_NETWORK_NEURAL_NETWORK_H

/**
 * @file neural_network.h
 * @brief Neural network implementation supporting feed-forward propagation
 * @defgroup neural_network Neural Network Components
 * @{
 */

#include "neural_network/layer_topology.h"  // NOLINT

namespace cshorelark::neural_network {

// Forward declarations
template <typename T = float>
class neuron;

template <typename T = float>
class layer;

template <typename T = float>
class network;

class layer_topology;

}  // namespace cshorelark::neural_network

/** @} */

#endif  // NEURAL_NETWORK_NEURAL_NETWORK_H_