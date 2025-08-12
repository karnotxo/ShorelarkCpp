#include "simulation/simulation_error.h"

namespace cshorelark::simulation {

auto simulation_error_to_string(simulation_error error) -> const char* {
    switch (error) {
        case simulation_error::k_none:
            return "No error";
        case simulation_error::k_neural_network_error:
            return "Generic neural network error";
        case simulation_error::k_invalid_network_weights:
            return "Invalid neural network weights";
        case simulation_error::k_insufficient_outputs:
            return "Insufficient neural network outputs";
        case simulation_error::k_invalid_input_size:
            return "Invalid input size for neural network";
        case simulation_error::k_layer_propagation_failed:
            return "Layer propagation failed in neural network";
        case simulation_error::k_invalid_network_structure:
            return "Invalid neural network structure";
        case simulation_error::k_world_bounds_error:
            return "World bounds error";
        case simulation_error::k_configuration_error:
            return "Configuration error";
        case simulation_error::k_invalid_brain_config:
            return "Invalid brain configuration";
        case simulation_error::k_invalid_chromosome:
            return "Invalid chromosome for brain creation";
        case simulation_error::k_brain_operation_failed:
            return "Brain operation failed";
        default:
            return "Unknown simulation error";
    }
}

}  // namespace cshorelark::simulation
