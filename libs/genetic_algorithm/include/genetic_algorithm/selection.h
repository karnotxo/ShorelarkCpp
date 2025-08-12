#ifndef CSHORELARK_GENETIC_ALGORITHM_SELECTION_H
#define CSHORELARK_GENETIC_ALGORITHM_SELECTION_H

#include <cstddef>
#include <memory>
#include <nonstd/span.hpp>

#include "genetic_algorithm/genetic_error.h"
#include "genetic_algorithm/individual.h"
#include "random/random.h"
#include "tl/expected.hpp"

namespace cshorelark::genetic {

/**
 * @brief Interface for selection strategies in genetic algorithms
 *
 * Selection strategies determine how individuals are chosen for reproduction
 * based on their fitness.
 */
class selection_strategy {
public:
    /**
     * @brief Select an individual from the population
     *
     * @param population Span of individuals to select from
     * @param rng Random number generator
     * @return Unique pointer to a clone of the selected individual
     */
    [[nodiscard]] virtual auto select(nonstd::span<const std::unique_ptr<individual>> population,
                                      cshorelark::random::random_generator& random) const
       -> tl::expected<const size_t, genetic_error> = 0;       

};

/**
 * @brief Tournament selection strategy
 *
 * This strategy selects the best individual from a random subset of the population.
 */
class tournament_selection : public selection_strategy {
public:
    /**
     * @brief Constructor
     *
     * @param tournament_size Number of individuals to include in each tournament
     * @param reversed If true, lower fitness values are considered better
     */
    explicit tournament_selection(size_t tournament_size = 3, bool reversed = false)
        : tournament_size_(tournament_size), reversed_(reversed) {}

    /**
     * @brief Select an individual using tournament selection
     *
     * @param population Span of individuals to select from
     * @param rng Random number generator
     * @return Unique pointer to a clone of the selected individual
     */
    [[nodiscard]] auto select(nonstd::span<const std::unique_ptr<individual>> population,
                              cshorelark::random::random_generator& random) const
        -> tl::expected<const size_t, genetic_error> override;

private:
    size_t tournament_size_;  ///< Number of individuals to include in each tournament
    bool reversed_;           ///< If true, lower fitness values are considered better
};

/**
 * @brief Roulette wheel (fitness proportionate) selection strategy
 *
 * This strategy selects individuals with probability proportional to their fitness.
 */
class roulette_wheel_selection : public selection_strategy {
public:
    /**
     * @brief Constructor
     *
     * @param reversed If true, lower fitness values are considered better
     */
    explicit roulette_wheel_selection(bool reversed = false) : reversed_(reversed) {}

    /**
     * @brief Select an individual using roulette wheel selection
     *
     * @param population Span of individuals to select from
     * @param rng Random number generator
     * @return Unique pointer to a clone of the selected individual
     */
    [[nodiscard]] auto select(nonstd::span<const std::unique_ptr<individual>> population,
                              cshorelark::random::random_generator& random) const
        -> tl::expected<const size_t, genetic_error> override;

private:
    bool reversed_;  ///< If true, lower fitness values are considered better
};

}  // namespace cshorelark::genetic

#endif  // CSHORELARK_GENETIC_ALGORITHM_SELECTION_H