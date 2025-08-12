#ifndef CSHORELARK_SIMULATION_CONFIG_H
#define CSHORELARK_SIMULATION_CONFIG_H

// C++ system headers
#include <cstddef>

// Project headers
#include "simulation/constants.h"

namespace cshorelark::simulation {

using namespace constants;

/**
 * @brief Configuration for the world
 */
struct world_config {
    std::size_t num_foods = 60;    ///< Number of food items in the world
    std::size_t num_animals = 40;  ///< Number of animals in the world
    float food_size = 0.01F;       ///< Size of food items (1% of world size)
    float bird_size = 0.015F;      ///< Size of bird items (1% of world size)
};

/**
 * @brief Configuration for an animal's eye
 */
struct brain_eye_config {
    float fov_range = 0.25F;       ///< Range of vision (25% of world size)
    float fov_angle_deg = 225.0F;  ///< Field of view angle in degrees
    std::size_t num_cells = 9;     ///< Number of "photoreceptors"
    std::size_t num_neurons = 9;   ///< Brain neurons
};

/**
 * @brief Configuration for an animal
 */
struct sim_config {
    float speed_min = 0.001F;              ///< Minimum speed (0.1% per step)
    float speed_max = 0.005F;              ///< Maximum speed (0.5% per step)
    float speed_accel = 0.2F;              ///< Speed acceleration (0.2% per step)
    float rotation_accel_deg = 90.0F;      ///< Rotation acceleration
    std::size_t generation_length = 2500;  ///< Length of a generation
};

/**
 * @brief Configuration for the genetic algorithm
 */
struct genetic_config {
    // Default values from Rust implementation
    float mutation_chance = 0.01F;  ///< Chance of mutation (1%)
    float mutation_coeff = 0.3F;    ///< Coefficient for mutation strength
    bool reverse = false;           ///< Whether to reverse the selection process
};

/**
 * @brief Configuration for the entire simulation
 */
struct config {
    // Default values from Rust implementation:
    // https://github.com/Patryk27/shorelark/blob/main/libs/simulation/src/config.rs

    brain_eye_config brain_eye;  ///< Brain and Eye configuration
    genetic_config genetic;      ///< Genetic algorithm configuration
    sim_config sim;              ///< Simulation configuration
    world_config world;          ///< World configuration
};

}  // namespace cshorelark::simulation

#endif  // CSHORELARK_SIMULATION_CONFIG_H