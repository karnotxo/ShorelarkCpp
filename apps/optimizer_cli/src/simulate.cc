#include "simulate.h"

#include <spdlog/spdlog.h>
#include <transwarp.h>

#include <array>
#include <atomic>
#include <chrono>
#include <cmath>
#include <fstream>
#include <mutex>
#include <nlohmann/json.hpp>
#include <thread>

#include "common.h"
#include "simulation/config.h"
#include "simulation/simulation.h"

namespace cshorelark::optimizer_cli {

namespace tw = transwarp;
using json = nlohmann::json;

simulation_runner::simulation_runner(size_t iterations, size_t generations,
                                     std::filesystem::path output_path)
    : iterations_(iterations), generations_(generations), output_path_(std::move(output_path)) {}

void simulation_runner::run() {
    spdlog::info("Starting neural network optimization simulation");
    spdlog::info("Generations: {}", generations_);
    spdlog::info("Iterations: {}", iterations_);

    try {
        // Generate all parameter combinations to test
        auto combinations = generate_combinations();
        size_t total_steps = combinations.size() * iterations_;
        spdlog::info(
            "Testing {} parameter combinations with {} iterations each ({} total simulations)",
            combinations.size(), iterations_, total_steps);

        // Setup tracking for completed steps
        std::atomic<size_t> done_steps{0};
        std::atomic<bool> is_done{false};

        // Start progress monitoring in a separate thread
        auto start_time = std::chrono::steady_clock::now();
        std::thread progress_thread(monitor_progress, total_steps, std::ref(done_steps),
                                    std::ref(is_done), start_time);

        // Prepare storage for results with thread safety
        std::vector<simulation_log_entry> log_entries;
        std::mutex log_mutex;

        // Create the executor with hardware concurrency
        transwarp::parallel executor(std::thread::hardware_concurrency());

        // Create tasks for each combination
        std::vector<std::shared_ptr<transwarp::task<void>>> tasks;
        tasks.reserve(combinations.size());

        for (const auto& params : combinations) {
            // Create a task for each parameter combination
            auto task = transwarp::make_task(
                transwarp::root, [this, params, &log_entries, &log_mutex, &done_steps]() {
                    this->run_simulation(params, log_entries, log_mutex, done_steps);
                });
            task->schedule(executor);
            // Store the task in the vector
            tasks.push_back(std::move(task));
        }

        // Wait for all tasks to complete
        for (auto& task : tasks) {
            task->get();
        }

        // Mark processing as complete and wait for progress thread
        is_done = true;
        if (progress_thread.joinable()) {
            progress_thread.join();
        }

        // Save results and report
        auto result = save_results(log_entries);
        if (result) {
            spdlog::info("{}", *result);
        } else {
            spdlog::error("Failed to save results: {}", result.error());
        }

    } catch (const std::exception& e) {
        spdlog::error("Error during simulation: {}", e.what());
    }
}

void simulation_runner::run_simulation(const simulation::config& sim_config,
                                       std::vector<simulation_log_entry>& log_entries,
                                       std::mutex& log_mutex, std::atomic<size_t>& done_steps) {
    // Initialize random generator
    cshorelark::random::random_generator random;

    // Collect statistics for multiple iterations
    for (size_t iter = 0; iter < iterations_; ++iter) {
        // Create random simulation instance - equivalent to Rust's let mut sim =
        // Simulation::random(config, &mut rng);
        auto sim = simulation::simulation::random(sim_config, random);

        // Run the simulation for specified number of generations
        for (size_t gen = 0; gen < generations_; ++gen) {
            // Train the simulation - equivalent to Rust's let stats = sim.train(&mut rng);
            auto stats = sim.train(random);

            // Record statistics
            simulation_log_entry entry{
                sim_config, gen, iter, stats  // Directly assign stats to entry.stats
            };

            spdlog::info("Iteration: {}, Generation: {}, Stats: {}", iter, gen, stats);

            // Thread-safe addition to log entries
            {
                std::lock_guard<std::mutex> lock(log_mutex);
                log_entries.push_back(std::move(entry));
            }
        }

        // Increment completed steps counter
        done_steps.fetch_add(1, std::memory_order_relaxed);
    }
}

auto simulation_runner::generate_combinations() -> std::vector<simulation::config> {
    // Get parameter options from config
    const parameter_options options;
    std::vector<simulation::config> combinations;

    // Using nested loops to generate all combinations
    // Similar to nested for loops in the Rust implementation
    for (auto brain_neurons : options.brain_neurons) {
        for (auto eye_fov_range : options.eye_fov_range) {
            for (auto eye_fov_angle_deg : options.eye_fov_angle_deg) {
                for (auto eye_cells : options.eye_cells) {
                    for (auto mutation_chance : options.ga_mut_chance) {
                        for (auto mutation_coeff : options.ga_mut_coeff) {
                            // Create parameter set
                            simulation::config params;
                            params.brain_eye.num_neurons = brain_neurons;
                            params.brain_eye.fov_angle_deg = eye_fov_range;
                            params.brain_eye.fov_angle_deg = eye_fov_angle_deg;
                            params.brain_eye.num_cells = eye_cells;
                            params.genetic.mutation_chance = mutation_chance;
                            params.genetic.mutation_coeff = mutation_coeff;

                            // Add to combinations
                            combinations.push_back(params);
                        }
                    }
                }
            }
        }
    }

    return combinations;
}

auto simulation_runner::save_results(const std::vector<simulation_log_entry>& log_entries) const
    -> tl::expected<std::string, std::string> {
    try {
        // Create the main JSON array to hold all log entries
        json logs_json = json::array();

        // Process each log entry
        for (const auto& entry : log_entries) {
            // Create a JSON object for this log entry (OptLog)
            json log_entry;

            // Add configuration (OptConfig with renamed fields)
            json config;
            config["c"] =
                static_cast<uint8_t>(entry.config.brain_eye.num_neurons);         // brain_neurons
            config["d"] = entry.config.brain_eye.fov_range;                       // eye_fov_range
            config["e"] = entry.config.brain_eye.fov_angle_deg;                   // eye_fov_angle
            config["f"] = static_cast<size_t>(entry.config.brain_eye.num_cells);  // eye_cells
            config["g"] = entry.config.genetic.mutation_chance;                   // ga_mut_chance
            config["h"] = entry.config.genetic.mutation_coeff;                    // ga_mut_coeff
            log_entry["cfg"] = config;

            // Add context (OptContext with renamed fields)
            json context;
            context["g"] = entry.generation;  // gen
            context["i"] = entry.iteration;   // iter
            log_entry["ctxt"] = context;

            // Add statistics (OptStatistics with renamed fields)
            json stats;
            stats["a"] = entry.stats.ga_stats().min_fitness();     // min_fitness
            stats["b"] = entry.stats.ga_stats().max_fitness();     // max_fitness
            stats["c"] = entry.stats.ga_stats().avg_fitness();     // avg_fitness
            stats["d"] = entry.stats.ga_stats().median_fitness();  // median_fitness
            log_entry["stats"] = stats;

            // Add this log entry to the array
            logs_json.push_back(log_entry);
        }

        // Write to file
        std::ofstream file(output_path_);
        if (!file) {
            return tl::make_unexpected("Failed to open output file: " + output_path_.string());
        }

        // Write the JSON with proper formatting
        file << logs_json.dump(2);

        return std::string("Results saved to: ") + output_path_.string();
    } catch (const std::exception& e) {
        return tl::make_unexpected(std::string("Error saving results: ") + e.what());
    }
}

void simulation_runner::monitor_progress(
    size_t total_steps, std::atomic<size_t>& done_steps, std::atomic<bool>& is_done,
    std::chrono::time_point<std::chrono::steady_clock> started_at) {
    // Update interval in milliseconds
    constexpr auto update_interval = std::chrono::milliseconds(500);

    while (!is_done) {
        // Sleep for update interval
        std::this_thread::sleep_for(update_interval);

        // Get current progress
        size_t current_steps = done_steps.load(std::memory_order_relaxed);

        // Calculate percentage and elapsed time
        double percentage = (static_cast<double>(current_steps) / total_steps) * 100.0;
        auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(
                           std::chrono::steady_clock::now() - started_at)
                           .count();

        // Calculate ETA
        std::string eta_str = "unknown";
        if (current_steps > 0) {
            double steps_per_second = static_cast<double>(current_steps) / elapsed;
            if (steps_per_second > 0) {
                long eta_seconds =
                    static_cast<long>((total_steps - current_steps) / steps_per_second);
                eta_str = std::to_string(eta_seconds) + "s";
            }
        }

        // Log progress
        spdlog::info("Progress: {}/{} ({:.1f}%) - Elapsed: {}s - ETA: {}", current_steps,
                     total_steps, percentage, elapsed, eta_str);
    }

    // Final progress update
    size_t final_steps = done_steps.load(std::memory_order_relaxed);
    auto total_elapsed = std::chrono::duration_cast<std::chrono::seconds>(
                             std::chrono::steady_clock::now() - started_at)
                             .count();

    spdlog::info("Completed {}/{} simulations in {}s", final_steps, total_steps, total_elapsed);
}

}  // namespace cshorelark::optimizer_cli
