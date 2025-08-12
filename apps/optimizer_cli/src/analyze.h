/**
 * @file analyze.hpp
 * @brief Analysis utilities for optimization results
 */
#ifndef CSHORELARK_OPTIMIZER_CLI_ANALYZE_H
#define CSHORELARK_OPTIMIZER_CLI_ANALYZE_H

#include <cstddef>
#include <filesystem>
#include <nonstd/span.hpp>
#include <string>
#include <tl/expected.hpp>
#include <vector>

namespace cshorelark::optimizer_cli::analyze {

/**
 * @brief Statistics about fitness values
 *
 * This struct is used to store statistics about the fitness values
 * of a population, matching the structure in the Rust implementation.
 */
struct stats {
    float min_fitness;     ///< Minimum fitness value
    float max_fitness;     ///< Maximum fitness value
    float avg_fitness;     ///< Average fitness value
    float median_fitness;  ///< Median fitness value
};

/**
 * @brief Computes statistics from a collection of fitness values
 *
 * @param fitnesses Span of fitness values
 * @return Stats object containing computed statistics
 */
auto compute_stats(nonstd::span<const float> fitnesses) -> stats;

/**
 * @brief Finds the index of the individual with the highest fitness
 *
 * @param fitnesses Span of fitness values
 * @return Index of the individual with the highest fitness
 */
auto find_best_individual_idx(nonstd::span<const float> fitnesses) -> std::size_t;

/**
 * @brief Run analysis on the optimization log
 *
 * @param input_path Path to input file with optimization data
 * @param output_path Optional path to save analysis output
 * @return Success message or error
 */
auto run_analysis(const std::filesystem::path& input_path,
                  const std::filesystem::path& output_path = std::filesystem::path())
    -> tl::expected<std::string, std::string>;

}  // namespace cshorelark::optimizer_cli::analyze

#endif  // CSHORELARK_OPTIMIZER_CLI_ANALYZE_H