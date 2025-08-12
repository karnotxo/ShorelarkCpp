#ifndef CSHORELARK_GENETIC_ALGORITHM_CROSSOVER_H
#define CSHORELARK_GENETIC_ALGORITHM_CROSSOVER_H

#include <memory>

#include "genetic_algorithm/chromosome.h"
#include "genetic_algorithm/genetic_error.h"
#include "random/random.h"

namespace cshorelark::genetic {

/**
 * @brief Interface for crossover strategies in genetic algorithms
 *
 * Crossover strategies define how genetic material is exchanged between
 * parent individuals to produce offspring.
 */
class crossover_strategy {
public:
    /**
     * @brief Virtual destructor
     */
    virtual ~crossover_strategy() = default;

    /**
     * @brief Perform crossover between two parent individuals
     *
     * @param parent1 First parent individual
     * @param parent2 Second parent individual
     * @param rng Random number generator
     * @return Pair of unique pointers to the offspring individuals
     */
    [[nodiscard]] virtual auto crossover(const chromosome& parent1, const chromosome& parent2,
                                         cshorelark::random::random_generator& random) const
        -> tl::expected<chromosome, genetic_error> = 0;
};

/**
 * @brief Single-point crossover strategy
 *
 * This strategy exchanges genetic material by selecting a single crossover point
 * and swapping all genes after that point.
 */
class single_point_crossover : public crossover_strategy {
public:
    /**
     * @brief Perform single-point crossover between two parent individuals
     *
     * @param parent1 First parent individual
     * @param parent2 Second parent individual
     * @param rng Random number generator
     * @return Pair of unique pointers to the offspring individuals
     */
    [[nodiscard]] auto crossover(const chromosome& parent1, const chromosome& parent2,
                                 cshorelark::random::random_generator& random) const
        -> tl::expected<chromosome, genetic_error> override;
};

/**
 * @brief Uniform crossover strategy
 *
 * This strategy exchanges genetic material by randomly selecting genes from
 * either parent for each position in the offspring.
 */
class uniform_crossover : public crossover_strategy {
public:
    /**
     * @brief Constructor
     *
     * @param swap_probability Probability of swapping genes at each position
     */
    explicit uniform_crossover(float swap_probability = 0.5F)
        : swap_probability_(swap_probability) {}

    /**
     * @brief Perform uniform crossover between two parent individuals
     *
     * @param parent1 First parent individual
     * @param parent2 Second parent individual
     * @param rng Random number generator
     * @return Pair of unique pointers to the offspring individuals
     */
    [[nodiscard]] auto crossover(const chromosome& parent1, const chromosome& parent2,
                                 cshorelark::random::random_generator& random) const
        -> tl::expected<chromosome, genetic_error> override;

private:
    float swap_probability_;  ///< Probability of swapping genes at each position
};

}  // namespace cshorelark::genetic

#endif  // CSHORELARK_GENETIC_ALGORITHM_CROSSOVER_H