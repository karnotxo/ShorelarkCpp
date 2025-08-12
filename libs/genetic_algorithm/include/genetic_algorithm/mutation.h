#ifndef CSHORELARK_GENETIC_ALGORITHM_MUTATION_H
#define CSHORELARK_GENETIC_ALGORITHM_MUTATION_H

#include <memory>

#include "genetic_algorithm/chromosome.h"
#include "genetic_algorithm/genetic_error.h"
#include "random/random.h"
#include "tl/expected.hpp"

namespace cshorelark::genetic {

/**
 * @brief Interface for mutation strategies in genetic algorithms
 *
 * Mutation strategies define how genetic material is randomly altered
 * to maintain genetic diversity.
 */
class mutation_strategy {
public:
    /**
     * @brief Virtual destructor
     */
    virtual ~mutation_strategy() = default;

    /**
     * @brief Perform mutation on an individual
     *
     * @param individual Individual to mutate
     * @param rng Random number generator
     * @return Unique pointer to the mutated individual
     */
    [[nodiscard]] virtual auto mutate(chromosome& child,
                                      cshorelark::random::random_generator& random) const
        -> tl::expected<void, genetic_error> = 0;
};

/**
 * @brief Gaussian mutation strategy
 *
 * This strategy adds Gaussian noise to genes with a given probability.
 */
class gaussian_mutation : public mutation_strategy {
public:
    /**
     * @brief Constructor
     *
     * @param mutation_probability Probability of mutating each gene
     * @param mutation_strength Standard deviation of the Gaussian noise
     */
    explicit gaussian_mutation(float mutation_probability = 0.01F, float mutation_strength = 0.1F) noexcept;

    /**
     * @brief Perform Gaussian mutation on an individual
     *
     * @param individual Individual to mutate
     * @param rng Random number generator
     * @return Unique pointer to the mutated individual
     */
    [[nodiscard]] auto mutate(chromosome& child, cshorelark::random::random_generator& random) const
        -> tl::expected<void, genetic_error> override;

private:
    float mutation_probability_;  ///< Probability of mutating each gene
    float mutation_strength_;     ///< Standard deviation of the Gaussian noise
};

/**
 * @brief Uniform mutation strategy
 *
 * This strategy replaces genes with random values within a given range.
 */
class uniform_mutation : public mutation_strategy {
public:
    /**
     * @brief Constructor
     *
     * @param mutation_probability Probability of mutating each gene
     * @param min_value Minimum value for random replacement
     * @param max_value Maximum value for random replacement
     */
    explicit uniform_mutation(float mutation_probability = 0.01F, float min_value = -1.0F,
                              float max_value = 1.0F) noexcept;

    /**
     * @brief Perform uniform mutation on an individual
     *
     * @param individual Individual to mutate
     * @param rng Random number generator
     * @return Unique pointer to the mutated individual
     */
    [[nodiscard]] auto mutate(chromosome& child, cshorelark::random::random_generator& random) const
        -> tl::expected<void, genetic_error> override;

private:
    float mutation_probability_;  ///< Probability of mutating each gene
    float min_value_;             ///< Minimum value for random replacement
    float max_value_;             ///< Maximum value for random replacement
};

}  // namespace cshorelark::genetic

#endif  // CSHORELARK_GENETIC_ALGORITHM_MUTATION_H