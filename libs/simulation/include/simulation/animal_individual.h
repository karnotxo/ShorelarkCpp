#ifndef CSHORELARK_SIMULATION_ANIMAL_INDIVIDUAL_H
#define CSHORELARK_SIMULATION_ANIMAL_INDIVIDUAL_H

// C++ system headers
#include <cstddef>
#include <memory>
#include <tl/expected.hpp>

// Project headers
#include "genetic_algorithm/chromosome.h"
#include "genetic_algorithm/genetic_error.h"
#include "genetic_algorithm/individual.h"
#include "random/random.h"
#include "simulation/animal.h"
#include "simulation/config.h"

namespace cshorelark::simulation {

/**
 * @brief Concrete individual implementation for animal simulation
 */
class animal_individual : public genetic::individual {
public:
    /**
     * @brief Constructs an animal individual with the given chromosome and fitness
     * @param chromosome Chromosome containing the neural network weights
     */
    explicit animal_individual(genetic::chromosome&& chromosome);

    /**
     * @brief Constructs an animal individual with the given chromosome and fitness
     * @param food_eaten Fitness value of this individual
     * @param chromosome Chromosome containing the neural network weights
     */
    animal_individual(std::size_t food_eaten, genetic::chromosome&& chromosome);

    // Implement Individual interface

    [[nodiscard]] auto get_chromosome() const noexcept -> const genetic::chromosome& override;
    [[nodiscard]] auto get_fitness() const noexcept -> float override;
    /**
     * @brief Inverts the food eaten counter
     * @param max_value Maximum value for the food eaten counter
     */
    void invert_fitness(const size_t max_value) { food_eaten_ = max_value - food_eaten_; }

    /**
     * @brief Creates an animal_individual from an animal
     * @param animal The animal to convert
     * @return A new animal_individual
     */
    [[nodiscard]] static auto from_animal(const animal& animal) -> animal_individual;

    [[nodiscard]] auto into_animal(const config& config,
                                   cshorelark::random::random_generator& random) -> animal;

    /**
     * @brief Static factory method to create a new animal_individual from a chromosome
     * @param chromosome The chromosome to use (moved into the new individual)
     * @return A unique_ptr to a new animal_individual with the given chromosome, or an error
     */
    [[nodiscard]] static auto from_chromosome(genetic::chromosome&& chromosome)
        -> tl::expected<std::unique_ptr<individual>, genetic::genetic_error>;

private:
    genetic::chromosome chromosome_;  // Store chromosome as member (non-const to allow moving)
    std::size_t food_eaten_{0};       // Track food eaten count
};  // class animal_individual

}  // namespace cshorelark::simulation

#endif  // CSHORELARK_SIMULATION_ANIMAL_INDIVIDUAL_H