#ifndef CSHORELARK_GENETIC_ALGORITHM_STATISTICS_H
#define CSHORELARK_GENETIC_ALGORITHM_STATISTICS_H

#include <algorithm>
#include <cstddef>
#include <memory>
#include <nonstd/span.hpp>
#include <vector>

#include "genetic_algorithm/individual.h"

namespace cshorelark::genetic {

/**
 * @brief Statistics collected during genetic algorithm evolution
 *
 * This class provides statistics about the fitness of a population,
 * such as minimum, maximum, average, and median fitness values.
 */
class statistics {
public:
    /**
     * @brief Construct statistics with pre-calculated values
     *
     * @param min Minimum fitness value
     * @param max Maximum fitness value
     * @param avg Average fitness value
     * @param median Median fitness value
     */
    statistics(float min, float max, float avg, float median)
        : min_fitness_(min), max_fitness_(max), avg_fitness_(avg), median_fitness_(median) {}

    /**
     * @brief Calculate statistics from a population of individuals
     *
     * @param population Span of pointers to individuals
     * @return Statistics object
     */
    static auto from_population(nonstd::span<const std::unique_ptr<individual>> population)
        -> statistics {
        // Check if the population is empty
        if (population.empty()) {
            return statistics(0.0f, 0.0f, 0.0f, 0.0f);  // Handle empty population case
        }
        // Collection of fitness values
        std::vector<float> fitness_values;
        fitness_values.reserve(population.size());

        // Calculate sum for average
        float sum = 0.0f;

        // Find min and max
        float min_fitness = std::numeric_limits<float>::max();
        float max_fitness = std::numeric_limits<float>::lowest();

        // Extract all fitness values
        for (const auto& individual : population) {
            float fitness = individual->get_fitness();

            min_fitness = std::min(min_fitness, fitness);
            max_fitness = std::max(max_fitness, fitness);
            sum += fitness;

            fitness_values.push_back(fitness);
        }

        // Calculate average
        float avg_fitness = sum / static_cast<float>(population.size());

        // Calculate median
        std::sort(fitness_values.begin(), fitness_values.end());
        float median_fitness;

        if (fitness_values.empty()) {
            median_fitness = 0.0f;
        } else if (fitness_values.size() % 2 == 0) {
            // Even number of elements
            size_t mid = fitness_values.size() / 2;
            median_fitness = (fitness_values[mid - 1] + fitness_values[mid]) / 2.0f;
        } else {
            // Odd number of elements
            median_fitness = fitness_values[fitness_values.size() / 2];
        }

        return {min_fitness, max_fitness, avg_fitness, median_fitness};
    }

    // Accessor methods
    [[nodiscard]] auto min_fitness() const noexcept -> float { return min_fitness_; }
    [[nodiscard]] auto max_fitness() const noexcept -> float { return max_fitness_; }
    [[nodiscard]] auto avg_fitness() const noexcept -> float { return avg_fitness_; }
    [[nodiscard]] auto median_fitness() const noexcept -> float { return median_fitness_; }

private:
    float min_fitness_;     ///< Minimum fitness in the population
    float max_fitness_;     ///< Maximum fitness in the population
    float avg_fitness_;     ///< Average fitness in the population
    float median_fitness_;  ///< Median fitness in the population
};

}  // namespace cshorelark::genetic

#endif  // CSHORELARK_GENETIC_ALGORITHM_STATISTICS_H