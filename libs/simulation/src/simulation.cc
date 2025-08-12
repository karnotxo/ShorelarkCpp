#include "simulation/simulation.h"

#include <algorithm>
#include <cmath>
#include <memory>
#include <optional>
#include <vector>

#include "genetic_algorithm/crossover.h"
#include "genetic_algorithm/genetic_algorithm.h"
#include "genetic_algorithm/individual.h"
#include "genetic_algorithm/mutation.h"
#include "genetic_algorithm/selection.h"
#include "simulation/animal.h"
#include "simulation/animal_individual.h"
#include "simulation/food.h"
#include "simulation/statistics.h"
#include "spdlog/spdlog.h"

// No need for explicit template instantiation anymore since the template is defined in the header

namespace cshorelark::simulation {

using cshorelark::genetic::roulette_wheel_selection;
using cshorelark::random::random_generator;
using cshorelark::simulation::statistics;

simulation::simulation(config config, world&& world)
    : config_(config), world_(std::move(world)), age_(0), generation_(0) {}

auto simulation::random(const config& config, random_generator& random) -> simulation {
    world world = world::random(config, random);
    return simulation(config, std::move(world));
}

void simulation::spawn_food(cshorelark::random::random_generator& random) {
    float pos_x = random.generate_position();
    float pos_y = random.generate_position();
    spawn_food(pos_x, pos_y);
}

void simulation::spawn_food(float pos_x, float pos_y) {
    food new_food(vector2d{pos_x, pos_y});
    world_.get_foods().push_back(std::move(new_food));
}

void simulation::spawn_animal(cshorelark::random::random_generator& random) {
    animal new_animal = animal::random(config_, random);
    world_.get_animals().push_back(std::move(new_animal));
}

auto simulation::step(random_generator& random) -> std::optional<statistics> {
    process_collisions(random);
    process_brains();
    process_movements();
    return try_evolving(random);
}

auto simulation::train(random_generator& random) -> statistics {
    while (true) {
        if (auto stats = step(random)) {
            return *stats;
        }
    }
}

void simulation::process_collisions(random_generator& random) {
    for (auto& animal : world_.get_animals()) {
        for (auto& food : world_.get_foods()) {
            // Calculate distance between animal and food
            const float disx = animal.position().x() - food.position().x();
            const float disy = animal.position().y() - food.position().y();
            const float distance = std::sqrt(disx * disx + disy * disy);

            // If close enough, consider it eaten
            const float collision_distance = config_.world.food_size + config_.world.bird_size;
            if (distance <= collision_distance) {
                // Animal has a method to increment food_eaten
                animal.increment_food_eaten();
                // For now, ensure the food moves to a new random position
                food.randomize_position(random);
            }
        }
    }
}

void simulation::process_brains() {
    auto& foods = world_.get_foods();
    for (auto& animal : world_.get_animals()) {
        animal.process_brain(config_, foods);
    }
}

void simulation::process_movements() {
    for (auto& animal : world_.get_animals()) {
        animal.process_movement();
    }
}

auto simulation::try_evolving(random_generator& random) -> std::optional<statistics> {
    age_++;

    if (age_ > config_.sim.generation_length) {
        return evolve(random);
    }

    return std::nullopt;
}

auto simulation::evolve(cshorelark::random::random_generator& random) -> statistics {
    age_ = 0;
    generation_++;

    spdlog::debug("Evolving generation {}", generation_);

    // Convert animals to individuals for genetic algorithm
    std::vector<std::unique_ptr<cshorelark::genetic::individual>> individuals;
    individuals.reserve(world_.get_animals().size());

    // Create animal individuals - properly using move semantics to avoid copying
    for (const auto& animal : world_.get_animals()) {
        // Create an animal_individual from the animal
        auto individual = animal_individual::from_animal(animal);
        // Move the individual into the unique_ptr
        individuals.push_back(std::make_unique<animal_individual>(std::move(individual)));
    }

    // If genetic reverse is enabled, invert the fitness
    if (config_.genetic.reverse) {
        // Find max fitness for inversion
        const float max_fitness = std::max_element(individuals.begin(), individuals.end(),
                                                   [](const auto& a, const auto& b) {
                                                       return a->get_fitness() < b->get_fitness();
                                                   })
                                      ->get()
                                      ->get_fitness();

        // Invert fitness for each individual
        for (auto& individual : individuals) {
            // Cast to animal_individual to access the invert_fitness method
            auto* animal_ind = dynamic_cast<animal_individual*>(individual.get());
            if (animal_ind) {
                animal_ind->invert_fitness(max_fitness);
            }
        }
    }

    // Create genetic algorithm
    const cshorelark::genetic::genetic_algorithm<animal_individual> gen_algorithm(
        std::make_unique<roulette_wheel_selection>(),
        std::make_unique<genetic::uniform_crossover>(),
        std::make_unique<genetic::gaussian_mutation>(config_.genetic.mutation_chance,
                                                     config_.genetic.mutation_coeff));

    // Evolve the population
    auto evolved_result = gen_algorithm.evolve(individuals, random);
    if (!evolved_result) {
        spdlog::error("Evolution failed: error code {}, message: {}",
                      static_cast<int>(evolved_result.error().code),
                      evolved_result.error().message);
        throw std::runtime_error("Evolution failed");
    }

    // Access the successful result: first part is evolved individuals, second part is statistics
    auto [evolved_individuals, evolution_stats] = std::move(evolved_result.value());

    // Convert evolved individuals back to animals
    std::vector<animal> new_animals;
    new_animals.reserve(evolved_individuals.size());

    for (auto& individual : evolved_individuals) {
        // Downcast to animal_individual
        auto* animal_ind = dynamic_cast<animal_individual*>(individual.get());
        if (animal_ind != nullptr) {
            new_animals.push_back(animal_ind->into_animal(config_, random));
        }
    }

    // Replace the world's animals with the new generation
    world_.set_animals(std::move(new_animals));

    // Reset food positions
    for (auto& food : world_.get_foods()) {
        food.randomize_position(random);
    }

    return statistics{generation_ - 1, evolution_stats};
}

}  // namespace cshorelark::simulation

// Add explicit template instantiations to resolve linker errors
// This makes the compiler generate the code for these specific template instances
namespace cshorelark::genetic {
template class genetic_algorithm<cshorelark::simulation::animal_individual>;
}