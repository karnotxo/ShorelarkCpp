#ifndef CSHORELARK_SIMULATION_SIMULATION_ERROR_H
#define CSHORELARK_SIMULATION_SIMULATION_ERROR_H

namespace cshorelark::simulation {

/**
 * @brief Enumeration of possible simulation errors
 */
enum class simulation_error {
    k_none,                       ///< No error
    k_neural_network_error,       ///< Generic neural network error
    k_invalid_network_weights,    ///< Network weights are invalid or incompatible
    k_insufficient_outputs,       ///< Neural network did not produce enough outputs
    k_invalid_input_size,         ///< Input size mismatch for neural network
    k_layer_propagation_failed,   ///< Layer propagation failed in neural network
    k_invalid_network_structure,  ///< Neural network structure is invalid
    k_world_bounds_error,         ///< Error related to world boundaries
    k_configuration_error,        ///< Error in simulation configuration
    
    // Additional error types referenced in brain.cc
    k_invalid_brain_config,       ///< Invalid brain configuration
    k_invalid_chromosome,         ///< Invalid chromosome for brain creation
    k_brain_operation_failed,     ///< Brain operation failed
};

/**
 * @brief Gets a string representation of a simulation error
 * @param error The error to convert
 * @return String describing the error
 */
auto simulation_error_to_string(simulation_error error) -> const char*;

}  // namespace cshorelark::simulation

#endif  // CSHORELARK_SIMULATION_SIMULATION_ERROR_H