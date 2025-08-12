#ifndef CSHORELARK_GENETIC_ALGORITHM_GENETIC_ERROR_H
#define CSHORELARK_GENETIC_ALGORITHM_GENETIC_ERROR_H

/**
 * @file genetic_error.h
 * @brief Error types used in the genetic algorithm
 */

// C++ system headers
#include <string>

namespace cshorelark::genetic {

/**
 * @brief Error codes for genetic algorithm operations
 */
enum class genetic_error_code {
    k_invalid_parent_size,       ///< Parents have incompatible sizes
    k_invalid_population_size,   ///< Population size is invalid
    k_invalid_chromosome,        ///< Invalid chromosome
    k_invalid_selection,         ///< Selection operation failed
    k_selection_failed,          ///< Failed to select individual
    k_crossover_failed,          ///< Failed to perform crossover
    k_mutation_failed,           ///< Failed to perform mutation
    k_offspring_creation_failed  ///< Failed to create offspring individual
};

/**
 * @brief Error object for genetic algorithm operations
 */
struct genetic_error {
    genetic_error_code code;  ///< Error code
    std::string message;      ///< Error message
};

}  // namespace cshorelark::genetic

#endif  // CSHORELARK_GENETIC_ALGORITHM_GENETIC_ERROR_H_