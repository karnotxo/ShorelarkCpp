#include "simulation/animal_individual.h"

// C++ system headers
#include <cstddef>
#include <exception>  // Added for std::exception
#include <memory>     // Added for std::unique_ptr, std::make_unique
#include <string>     // Added for std::string
#include <utility>

// External libraries
#include <tl/expected.hpp>  // Added for tl::expected, tl::unexpected

// Project headers
#include "genetic_algorithm/chromosome.h"
#include "genetic_algorithm/genetic_error.h"  // Added for genetic_error, genetic_error_code
#include "genetic_algorithm/individual.h"
#include "random/random.h"
#include "simulation/config.h"

namespace cshorelark::simulation {

// Constructor to create an animal individual from a chromosome
animal_individual::animal_individual(genetic::chromosome&& chromosome)
    : chromosome_(std::move(chromosome)) {}

// Make sure fitness values are properly set in the constructor
animal_individual::animal_individual(std::size_t food_eaten, genetic::chromosome&& chromosome)
    : chromosome_(std::move(chromosome)), food_eaten_(food_eaten) {}

auto animal_individual::get_chromosome() const noexcept -> const genetic::chromosome& {
    return chromosome_;
}

auto animal_individual::get_fitness() const noexcept -> float {
    return static_cast<float>(food_eaten_);
}

[[nodiscard]] auto animal_individual::from_animal(const animal& animal) -> animal_individual {
    // Get the chromosome from the animal and immediately move it into the constructor
    return animal_individual{animal.food_eaten(), animal.as_chromosome()};
}

[[nodiscard]] auto animal_individual::into_animal(
    const config& config, cshorelark::random::random_generator& random) -> animal {
    // Clone the chromosome instead of moving it
    auto result = animal::from_chromosome(config, random, chromosome_);
    if (!result) {
        throw std::runtime_error("Failed to create animal from chromosome");
    }
    return std::move(result.value());
}

[[nodiscard]] auto animal_individual::from_chromosome(genetic::chromosome&& chromosome)
    -> tl::expected<std::unique_ptr<individual>, genetic::genetic_error> {
    try {
        // Create an animal_individual instance instead of a base individual instance
        std::unique_ptr<individual> new_individual =
            std::make_unique<animal_individual>(std::move(chromosome));
        return new_individual;
    } catch (const std::exception& e) {
        return tl::unexpected(
            genetic::genetic_error{genetic::genetic_error_code::k_invalid_chromosome,
                                   std::string("Failed to create animal_individual: ") + e.what()});
    }
}

}  // namespace cshorelark::simulation