#ifndef CSHORELARK_GENETIC_ALGORITHM_GENETIC_ALGORITHM_H
#define CSHORELARK_GENETIC_ALGORITHM_GENETIC_ALGORITHM_H

#include <memory>
#include <nonstd/span.hpp>
#include <type_traits>
#include <utility>
#include <vector>

#include "genetic_algorithm/chromosome.h"
#include "genetic_algorithm/crossover.h"
#include "genetic_algorithm/genetic_error.h"
#include "genetic_algorithm/individual.h"
#include "genetic_algorithm/mutation.h"
#include "genetic_algorithm/selection.h"
#include "genetic_algorithm/statistics.h"
#include "random/random.h"
#include "tl/expected.hpp"

namespace cshorelark::genetic {

/**
 * @brief Main genetic algorithm class that orchestrates the evolution process
 *
 * This class implements the core genetic algorithm logic using strategy objects
 * for selection, crossover, and mutation operations.
 */
template <class T>
class genetic_algorithm {
public:
    /**
     * @brief Construct a genetic algorithm with the specified strategies
     *
     * @param selection Selection strategy to use
     * @param crossover Crossover strategy to use
     * @param mutation Mutation strategy to use
     * @param reversed If true, inverts fitness comparisons (select worst instead of best)
     */
    genetic_algorithm(std::unique_ptr<selection_strategy> selection,
                      std::unique_ptr<crossover_strategy> crossover,
                      std::unique_ptr<mutation_strategy> mutation, bool reversed = false)
        : selection_(std::move(selection)),
          crossover_(std::move(crossover)),
          mutation_(std::move(mutation)),
          reversed_(reversed) {}

    /**
     * @brief Evolve a population to create the next generation
     *
     * @param population Current population of individuals
     * @param random_gen Random number generator
     * @return A pair containing the new population and statistics about the evolution
     */
    [[nodiscard]] auto evolve(nonstd::span<std::unique_ptr<individual>> population,
                              cshorelark::random::random_generator& random_gen) const
        -> tl::expected<std::pair<std::vector<std::unique_ptr<individual>>, statistics>,
                        genetic_error> {
        if (population.empty()) {
            return tl::make_unexpected(genetic_error{genetic_error_code::k_invalid_population_size,
                                                     "Population cannot be empty"});
        }
        // Create statistics for the current population
        const statistics stats = statistics::from_population(population);

        // Create next generation with same population size
        std::vector<std::unique_ptr<individual>> next_generation;
        next_generation.reserve(population.size());

        // Elite selection: keep the best individual
        /*if (!population.empty()) {
            // Find the best individual
            auto best_it = std::max_element(
                population.begin(), population.end(), [this](const auto& a, const auto& b) {
                    float fitness_a = a->get_fitness();
                    float fitness_b = b->get_fitness();
                    return reversed_ ? fitness_a > fitness_b : fitness_a < fitness_b;
                });

            // Add the best individual to the next generation
            next_generation.push_back((*best_it)->clone());
        }*/

        // Fill the rest of the next generation
        while (next_generation.size() < population.size()) {
            // Select parents
            auto parent_a_result = selection_->select(population, random_gen);
            if (!parent_a_result) {
                return tl::make_unexpected(parent_a_result.error());
            }

            auto parent_b_result = selection_->select(population, random_gen);
            if (!parent_b_result) {
                return tl::make_unexpected(parent_b_result.error());
            }

            // Get parent chromosomes
            const auto& parent_a = population[*parent_a_result];
            const auto& parent_b = population[*parent_b_result];
            const auto& parent_a_chromosome = parent_a->get_chromosome();
            const auto& parent_b_chromosome = parent_b->get_chromosome();

            // Perform crossover - handle tl::expected return value
            auto child_chromosome_result =
                crossover_->crossover(parent_a_chromosome, parent_b_chromosome, random_gen);

            if (!child_chromosome_result) {
                return tl::make_unexpected(child_chromosome_result.error());
            }

            auto child_chromosome = std::move(child_chromosome_result.value());

            // Mutate child - handle tl::expected return value
            auto mutated_result = mutation_->mutate(child_chromosome, random_gen);

            if (!mutated_result) {
                return tl::make_unexpected(mutated_result.error());
            }
            auto new_individual_result = create_individual(std::move(child_chromosome));
            if (!new_individual_result) {
                return tl::make_unexpected(new_individual_result.error());
            }
            next_generation.emplace_back(std::move(new_individual_result.value()));
        }

        return std::make_pair(std::move(next_generation), stats);
    }

protected:
    /**
     * @brief Create an object from a chromosome
     *
     * @param chromosome Chromosome to create the object from
     * @return An expected containing either the created object or a genetic error
     */
    static auto create_individual(genetic::chromosome&& chromosome)
        -> tl::expected<std::unique_ptr<individual>, genetic::genetic_error> {
        // Ensure T has a static from_chromosome method matching the expected signature
        static_assert(
            std::is_invocable_r_v<tl::expected<std::unique_ptr<individual>, genetic::genetic_error>,
                                  decltype(&T::from_chromosome), genetic::chromosome&&>,
            "T must have a static method 'from_chromosome(genetic::chromosome&&)' returning "
            "'tl::expected<std::unique_ptr<individual>, genetic::genetic_error>'");
        return T::from_chromosome(std::move(chromosome));
    }

private:
    std::unique_ptr<selection_strategy> selection_;  ///< Strategy for selecting individuals
    std::unique_ptr<crossover_strategy> crossover_;  ///< Strategy for crossover between individuals
    std::unique_ptr<mutation_strategy> mutation_;    ///< Strategy for mutating individuals
    bool reversed_;                                  ///< Whether to invert fitness comparisons
};

}  // namespace cshorelark::genetic

#endif  // CSHORELARK_GENETIC_ALGORITHM_GENETIC_ALGORITHM_H