#ifndef CSHORELARK_OPTIMIZER_CLI_SIMULATION_RUNNER_H
#define CSHORELARK_OPTIMIZER_CLI_SIMULATION_RUNNER_H

#include <atomic>
#include <chrono>
#include <cstddef>
#include <filesystem>
#include <memory>
#include <mutex>
#include <thread>
#include <tl/expected.hpp>
#include <vector>

#include "analyze.h"
#include "common.h"
#include "constants.h"
#include "genetic_algorithm/individual.h"
#include "neural_network/network.h"
#include "simulation/animal.h"
#include "simulation/config.h"
#include "simulation/statistics.h"
#include "simulation/world.h"

namespace cshorelark::optimizer_cli {

/**
 * @brief Log entry for simulation results
 */
struct simulation_log_entry {
    simulation::config config;     ///< Configuration used
    size_t generation = 0;         ///< Generation number
    size_t iteration = 0;          ///< Iteration number
    simulation::statistics stats;  ///< Statistics for this generation
};

/**
 * @brief Coordinates the optimization process with data saving
 */
class simulation_runner {
public:
    /**
     * @brief Constructor for the simulation runner
     *
     * @param iterations Number of iterations to run
     * @param generations Number of generations
     * @param output_path Path to save output files
     */
    simulation_runner(size_t iterations, size_t generations, std::filesystem::path output_path);

    /**
     * @brief Run the optimization process
     *
     * @return Expected containing status message or error
     */
    void run();

private:
    /**
     * @brief Generate all parameter combinations
     *
     * @return Vector of all parameter combinations to test
     */
    auto generate_combinations() -> std::vector<simulation::config>;

    /**
     * @brief Run a single simulation with specific parameters
     *
     * @param params Simulation parameters to use
     * @param log_entries Vector to store log entries
     * @param log_mutex Mutex to protect log entries vector
     * @param done_steps Counter for completed steps
     */
    void run_simulation(const simulation::config& params,
                        std::vector<simulation_log_entry>& log_entries, std::mutex& log_mutex,
                        std::atomic<size_t>& done_steps);

    /**
     * @brief Progress monitoring function that runs in a separate thread
     *
     * @param total_steps Total number of steps to complete
     * @param done_steps Reference to atomic counter of completed steps
     * @param is_done Flag indicating completion
     * @param started_at Start time of the process
     */
    static void monitor_progress(size_t total_steps, std::atomic<size_t>& done_steps,
                                 std::atomic<bool>& is_done,
                                 std::chrono::time_point<std::chrono::steady_clock> started_at);

    /**
     * @brief Save optimization results to output file
     *
     * @param log_entries Log entries to save
     * @return Expected containing success message or error
     */
    [[nodiscard]] auto save_results(const std::vector<simulation_log_entry>& log_entries) const
        -> tl::expected<std::string, std::string>;

    size_t iterations_;                  ///< Number of iterations to run
    size_t generations_;                 ///< Number of generations
    std::filesystem::path output_path_;  ///< Path to save output files
};

}  // namespace cshorelark::optimizer_cli

#endif  // CSHORELARK_OPTIMIZER_CLI_SIMULATION_RUNNER_H
