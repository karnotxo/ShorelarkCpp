#include "simulation/animal_individual.h"

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <vector>

#include "genetic_algorithm/chromosome.h"

using Catch::Matchers::WithinAbs;
using cshorelark::genetic::chromosome;
using cshorelark::simulation::animal_individual;

namespace {
// Helper to create a test chromosome
auto create_test_chromosome() -> chromosome {
    std::vector<float> genes(10, 0.5F);  // 10 genes, all set to 0.5
    return chromosome(std::move(genes));
}
}  // namespace

TEST_CASE("animal_individual - Initial state", "[animal_individual]") {
    auto test_chromosome = create_test_chromosome();
    animal_individual test_subject(std::move(test_chromosome));

    REQUIRE_THAT(test_subject.get_fitness(), WithinAbs(0.0F, 0.001F));
    REQUIRE(test_subject.get_chromosome().genes().size() == 10);
}

TEST_CASE("animal_individual - Fitness tracking", "[animal_individual]") {
    auto test_chromosome = create_test_chromosome();
    animal_individual test_subject(std::move(test_chromosome));

    // Create with initial food eaten count
    auto test_chromosome2 = create_test_chromosome();
    animal_individual test_subject2(42, std::move(test_chromosome2));

    REQUIRE_THAT(test_subject.get_fitness(), WithinAbs(0.0F, 0.001F));
    REQUIRE_THAT(test_subject2.get_fitness(), WithinAbs(42.0F, 0.001F));
}

TEST_CASE("animal_individual - Fitness inversion", "[animal_individual]") {
    auto test_chromosome = create_test_chromosome();
    animal_individual test_subject(42, std::move(test_chromosome));

    test_subject.invert_fitness(100);
    REQUIRE_THAT(test_subject.get_fitness(), WithinAbs(58.0F, 0.001F));
}

TEST_CASE("animal_individual - Create offspring", "[animal_individual]") {
    auto test_chromosome = create_test_chromosome();
    animal_individual parent(std::move(test_chromosome));

    auto new_chromosome = create_test_chromosome();
    auto offspring_result = animal_individual::from_chromosome(std::move(new_chromosome));

    REQUIRE(offspring_result.has_value());
    auto* offspring = dynamic_cast<animal_individual*>(offspring_result->get());
    REQUIRE(offspring != nullptr);
    REQUIRE_THAT(offspring->get_fitness(), WithinAbs(0.0F, 0.001F));
    REQUIRE(offspring->get_chromosome().genes().size() == 10);
}

TEST_CASE("animal_individual - From animal conversion", "[animal_individual]") {
    // Create a test animal with some food eaten
    auto config = cshorelark::simulation::config{};
    auto random = cshorelark::random::random_generator{};
    auto animal = cshorelark::simulation::animal::random(config, random);
    animal.increment_food_eaten();
    animal.increment_food_eaten();  // Now has eaten 2 food items

    auto individual = animal_individual::from_animal(animal);
    REQUIRE_THAT(individual.get_fitness(), WithinAbs(2.0F, 0.001F));
    REQUIRE(!individual.get_chromosome().genes().empty());
}