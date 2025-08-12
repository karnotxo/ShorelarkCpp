#ifndef CSHORELARK_OPTIMIZER_CONSTANTS_H
#define CSHORELARK_OPTIMIZER_CONSTANTS_H

#include <cstddef>

namespace cshorelark::optimizer {

// Common constants used throughout the optimizer package
namespace constants {
// Population configuration
constexpr std::size_t k_default_population_size = 50;

// Mutation parameters
constexpr float k_default_mutation_rate = 0.01F;
constexpr float k_default_mutation_amount = 0.3F;
constexpr float k_default_mutation_range = 1.0F;

// Selection parameters
constexpr std::size_t k_default_selection_count = 10;

// Alignment constants for performance
constexpr std::size_t k_alignment_value = 64;
constexpr std::size_t k_opt_log_alignment = 64;
constexpr std::size_t k_simulation_stats_alignment = 128;

// Iteration defaults
constexpr size_t k_default_iterations = 100;
}  // namespace constants

}  // namespace cshorelark::optimizer

#endif  // CSHORELARK_OPTIMIZER_CONSTANTS_H

#ifndef CSHORELARK_OPTIMIZER_CLI_CONSTANTS_H
#define CSHORELARK_OPTIMIZER_CLI_CONSTANTS_H

#include <cstddef>

namespace cshorelark::optimizer_cli::constants {

// Memory alignment values for better cache performance
constexpr std::size_t k_alignment_value = 16;
constexpr std::size_t k_simulation_stats_alignment = 16;

// Default values
constexpr std::size_t k_default_iterations = 15;
constexpr std::size_t k_default_generations = 30;

}  // namespace cshorelark::optimizer_cli::constants

#endif  // CSHORELARK_OPTIMIZER_CLI_CONSTANTS_H
