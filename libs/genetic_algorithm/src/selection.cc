/**
 * @file selection.cc
 * @brief Implementation of selection strateg    // Generate         do {
            // Use generate_position() to get a value between 0 and 1, then scale to population size
            candidate_index = static_cast<size_t>(random.generate_position() *
 static_cast<float>(population.size()));
            // Ensure we don't exceed bounds due to floating point precision
            if (candidate_index >= population.size()) {
                candidate_index = population.size() - 1;
            }random indices for tournament
 participants for (size_t i = 0; i < effective_tournament_size; ++i) { size_t candidate_index = 0;
        do {
            // Use generate_position() to get a value between 0 and 1, then scale to population size
            candidate_index = static_cast<size_t>(random.generate_position() *
 static_cast<float>(population.size()));
            // Ensure we don't exceed bounds due to floating point precision
            if (candidate_index >= population.size()) {
                candidate_index = population.size() - 1;
            }
        } while (std::find(tournament_indices.begin(), tournament_indices.end(), candidate_index)
                 != tournament_indices.end());
        tournament_indices.push_back(candidate_index);
    }c algorithms
 */

#include "genetic_algorithm/selection.h"

#include <algorithm>
#include <vector>

namespace cshorelark::genetic {

auto roulette_wheel_selection::select(nonstd::span<const std::unique_ptr<individual>> population,
                                      cshorelark::random::random_generator& random) const
    -> tl::expected<const size_t, genetic_error> {
    // The Rust implementation:
    // population
    //   .choose_weighted(rng, |individual| individual.fitness().max(0.00001))
    //   .expect("got an empty population")

    if (population.empty()) {
        return tl::unexpected(genetic_error{genetic_error_code::k_invalid_population_size,
                                            "Population must not be empty"});
    }

    // Calculate total fitness ensuring a minimum of 0.00001 for each individual (as in Rust)
    constexpr float k_min_individual_fitness = 0.00001F;
    float total_fitness = 0.0F;

    for (const auto& individual : population) {
        // Use max(fitness, k_min_individual_fitness) for each individual
        total_fitness += std::max(individual->get_fitness(), k_min_individual_fitness);
    }

    // Generate random point on the roulette wheel
    const float selection_point = random.generate_position() * total_fitness;

    // Find the selected individual
    float cumulative_fitness = 0.0F;
    for (auto i = 0; i < population.size(); ++i) {
        // Again use max(fitness, k_min_individual_fitness) for consistency
        cumulative_fitness += std::max(population[i]->get_fitness(), k_min_individual_fitness);
        if (cumulative_fitness >= selection_point) {
            return i;  // Return the index of the selected individual
        }
    }

    // Due to floating point rounding, we might not find an individual
    // In this case, return the last one (should be very rare)
    return population.size() - 1;
}

auto tournament_selection::select(nonstd::span<const std::unique_ptr<individual>> population,
                                  cshorelark::random::random_generator& random) const
    -> tl::expected<const size_t, genetic_error> {
    // Rust implementation reference:
    // population
    //   .choose_multiple(rng, self.size)
    //   .max_by_key(|individual| na::NotNan::new(individual.fitness()).expect("fitness was NaN"))
    //   .expect("got an empty population")

    if (population.empty()) {
        return tl::unexpected(genetic_error{genetic_error_code::k_invalid_population_size,
                                            "Population must not be empty"});
    }

    if (tournament_size_ == 0) {
        return tl::unexpected(genetic_error{genetic_error_code::k_invalid_parent_size,
                                            "Tournament size must be greater than zero"});
    }

    // Ensure tournament size doesn't exceed population size
    const size_t effective_tournament_size = std::min(tournament_size_, population.size());

    // Select random individuals for the tournament
    std::vector<size_t> tournament_indices;
    tournament_indices.reserve(effective_tournament_size);

    // Generate unique random indices for tournament participants
    for (size_t i = 0; i < effective_tournament_size; ++i) {
        size_t candidate_index;
        do {
            // Use generate_in_range() to get a value between 0 and population size
            candidate_index = static_cast<size_t>(
                random.generate_in_range(0.0F, static_cast<float>(population.size())));
            // Ensure we don't exceed bounds due to floating point precision
            if (candidate_index >= population.size()) {
                candidate_index = population.size() - 1;
            }
        } while (std::find(tournament_indices.begin(), tournament_indices.end(), candidate_index) !=
                 tournament_indices.end());
        tournament_indices.push_back(candidate_index);
    }

    // Find the best individual in the tournament
    size_t best_index = tournament_indices[0];
    float best_fitness = population[best_index]->get_fitness();

    for (size_t i = 1; i < tournament_indices.size(); ++i) {
        const size_t current_index = tournament_indices[i];
        const float current_fitness = population[current_index]->get_fitness();

        // Select based on whether we want reversed selection (lower is better) or not
        const bool is_better =
            reversed_ ? (current_fitness < best_fitness) : (current_fitness > best_fitness);

        if (is_better) {
            best_index = current_index;
            best_fitness = current_fitness;
        }
    }

    return best_index;
}

}  // namespace cshorelark::genetic