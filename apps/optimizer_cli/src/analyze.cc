#include "analyze.h"

#include <spdlog/spdlog.h>

#include <algorithm>
#include <exception>
#include <fstream>
#include <nlohmann/json.hpp>
#include <numeric>
#include <stdexcept>
#include <vector>

namespace cshorelark::optimizer_cli::analyze {

using json = nlohmann::json;

auto compute_stats(nonstd::span<const float> fitnesses) -> stats {
    if (fitnesses.empty()) {
        throw std::invalid_argument("Cannot compute statistics on empty fitness collection");
    }

    // Create a copy to sort (we don't want to modify the original data)
    std::vector<float> sorted_fitnesses(fitnesses.begin(), fitnesses.end());
    std::sort(sorted_fitnesses.begin(), sorted_fitnesses.end());

    const float min = sorted_fitnesses.front();
    const float max = sorted_fitnesses.back();

    // Calculate average
    const float sum = std::accumulate(sorted_fitnesses.begin(), sorted_fitnesses.end(), 0.0F);
    const float avg = sum / static_cast<float>(sorted_fitnesses.size());

    // Calculate median
    const float median = [&sorted_fitnesses]() {
        const std::size_t size = sorted_fitnesses.size();
        const std::size_t mid = size / 2;

        if (size % 2 == 0) {
            // Even number of elements, average the middle two
            return (sorted_fitnesses[mid - 1] + sorted_fitnesses[mid]) / 2.0F;
        } else {
            // Odd number of elements, return the middle one
            return sorted_fitnesses[mid];
        }
    }();

    return {min, max, avg, median};
}

auto find_best_individual_idx(nonstd::span<const float> fitnesses) -> std::size_t {
    if (fitnesses.empty()) {
        throw std::invalid_argument("Cannot find best individual in empty fitness collection");
    }

    return static_cast<std::size_t>(
        std::distance(fitnesses.begin(), std::max_element(fitnesses.begin(), fitnesses.end())));
}

auto run_analysis(const std::filesystem::path& input_path, const std::filesystem::path& output_path)
    -> tl::expected<std::string, std::string> {
    try {
        spdlog::info("Analyzing optimization log: {}", input_path.string());

        // Read JSON data from input file
        std::ifstream file(input_path);
        if (!file) {
            return tl::make_unexpected("Failed to open input file: " + input_path.string());
        }

        // Parse JSON data
        json input_data;
        try {
            file >> input_data;
        } catch (const json::exception& e) {
            return tl::make_unexpected(std::string("Failed to parse JSON: ") + e.what());
        }

        // Check if it's an array as expected
        if (!input_data.is_array()) {
            return tl::make_unexpected("Input file does not contain a JSON array");
        }

        // Create storage for fitness values by generation
        std::map<std::size_t, std::vector<float>> fitnesses_by_generation;

        // Extract data from each log entry
        for (const auto& entry : input_data) {
            // Extract context information to identify the generation
            auto generation = entry["ctxt"]["g"].get<std::size_t>();

            // Extract statistics
            // In the Rust code, the stats fields are renamed to a, b, c, d
            float min_fitness = entry["stats"]["a"].get<float>();
            float max_fitness = entry["stats"]["b"].get<float>();
            float avg_fitness = entry["stats"]["c"].get<float>();
            float median_fitness = entry["stats"]["d"].get<float>();

            // Store statistics by generation
            if (fitnesses_by_generation.find(generation) == fitnesses_by_generation.end()) {
                fitnesses_by_generation[generation] = {};
            }

            // Add this fitness stat to the generation
            fitnesses_by_generation[generation].push_back(max_fitness);
        }

        // Collect statistics across generations
        std::vector<float> min_fitness;
        std::vector<float> max_fitness;
        std::vector<float> avg_fitness;
        std::vector<float> median_fitness;

        // Calculate statistics for each generation
        for (const auto& [gen, fitnesses] : fitnesses_by_generation) {
            auto stats = compute_stats(fitnesses);
            min_fitness.push_back(stats.min_fitness);
            max_fitness.push_back(stats.max_fitness);
            avg_fitness.push_back(stats.avg_fitness);
            median_fitness.push_back(stats.median_fitness);
        }

        // Log summary statistics
        spdlog::info("Analyzed {} generations", fitnesses_by_generation.size());
        spdlog::info("Final generation statistics:");
        spdlog::info("  Min fitness: {:.4f}", min_fitness.back());
        spdlog::info("  Max fitness: {:.4f}", max_fitness.back());
        spdlog::info("  Avg fitness: {:.4f}", avg_fitness.back());
        spdlog::info("  Median fitness: {:.4f}", median_fitness.back());

        // Calculate improvement rates
        if (min_fitness.size() >= 2) {
            const float min_improvement = (min_fitness.back() - min_fitness.front()) /
                                          (static_cast<float>(min_fitness.size()) - 1.0F);

            const float max_improvement = (max_fitness.back() - max_fitness.front()) /
                                          (static_cast<float>(max_fitness.size()) - 1.0F);

            const float avg_improvement = (avg_fitness.back() - avg_fitness.front()) /
                                          (static_cast<float>(avg_fitness.size()) - 1.0F);

            spdlog::info("Improvement rates per generation:");
            spdlog::info("  Min fitness: {:.6f}", min_improvement);
            spdlog::info("  Max fitness: {:.6f}", max_improvement);
            spdlog::info("  Avg fitness: {:.6f}", avg_improvement);
        }

        // Save analysis results if output path is provided
        if (!output_path.empty()) {
            // Create JSON output for analysis
            json output_data;

            // Add configuration from the first entry
            if (!input_data.empty()) {
                output_data["config"] = input_data[0]["cfg"];
            }

            // Add analysis results
            json analysis;
            analysis["generations"] = fitnesses_by_generation.size();

            json final_stats;
            final_stats["min_fitness"] = min_fitness.back();
            final_stats["max_fitness"] = max_fitness.back();
            final_stats["avg_fitness"] = avg_fitness.back();
            final_stats["median_fitness"] = median_fitness.back();
            analysis["final_stats"] = final_stats;

            if (min_fitness.size() >= 2) {
                json improvement;
                improvement["min_fitness"] = (min_fitness.back() - min_fitness.front()) /
                                             (static_cast<float>(min_fitness.size()) - 1.0F);
                improvement["max_fitness"] = (max_fitness.back() - max_fitness.front()) /
                                             (static_cast<float>(max_fitness.size()) - 1.0F);
                improvement["avg_fitness"] = (avg_fitness.back() - avg_fitness.front()) /
                                             (static_cast<float>(avg_fitness.size()) - 1.0F);
                analysis["improvement_rates"] = improvement;
            }

            output_data["analysis"] = analysis;

            // Write to output file
            std::ofstream output_file(output_path);
            if (!output_file) {
                return tl::make_unexpected("Failed to open output file: " + output_path.string());
            }

            output_file << output_data.dump(2);  // Pretty print with 2-space indentation
            return std::string("Analysis results saved to: ") + output_path.string();
        }

        return std::string("Analysis completed successfully");

    } catch (const std::exception& e) {
        return tl::make_unexpected(std::string("Error analyzing optimization log: ") + e.what());
    }
}

}  // namespace cshorelark::optimizer_cli::analyze