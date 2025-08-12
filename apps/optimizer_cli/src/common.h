#ifndef CSHORELARK_OPTIMIZER_CLI_CONFIG_H
#define CSHORELARK_OPTIMIZER_CLI_CONFIG_H

#include <array>
#include <cstddef>

#include "constants.h"  // Project headers

namespace cshorelark::optimizer_cli {

/**
 * @brief Constant parameter values for generating combinations
 */
struct parameter_options {
    // Network parameters
    std::array<std::size_t, 4> brain_neurons = {2, 3, 5, 10};

    // Eye parameters
    std::array<float, 4> eye_fov_range = {0.1F, 0.25F, 0.33F, 0.5F};
    std::array<float, 4> eye_fov_angle_deg = {45.0F, 90.0F, 180.0F, 225.0F};
    std::array<std::size_t, 5> eye_cells = {2, 3, 6, 9, 12};

    // Genetic parameters
    std::array<float, 4> ga_mut_chance = {0.001F, 0.01F, 0.1F, 0.5F};
    std::array<float, 5> ga_mut_coeff = {0.01F, 0.1F, 0.3F, 0.5F, 1.0F};
};

}  // namespace cshorelark::optimizer_cli

#endif  // CSHORELARK_OPTIMIZER_CLI_CONFIG_H