#ifndef CSHORELARK_GENETIC_ALGORITHM_INDIVIDUAL_H
#define CSHORELARK_GENETIC_ALGORITHM_INDIVIDUAL_H

#include <memory>

#include "genetic_algorithm/chromosome.h"

namespace cshorelark::genetic {

/**
 * @brief Interface for individuals in a genetic algorithm
 *
 * This class defines the interface that all individuals must implement
 * to be compatible with the genetic algorithm framework.
 */
class individual {
public:
    /**
     * @brief Virtual destructor
     */
    virtual ~individual() = default;

    /**
     * @brief Get the fitness value of this individual
     *
     * @return Fitness score as a float
     */
    [[nodiscard]] virtual auto get_fitness() const -> float = 0;

    /**
     * @brief Get the chromosome representing this individual's genetic material
     *
     * @return Reference to the chromosome
     */
    [[nodiscard]] virtual auto get_chromosome() const -> const chromosome& = 0;
};

}  // namespace cshorelark::genetic

#endif  // CSHORELARK_GENETIC_ALGORITHM_INDIVIDUAL_H