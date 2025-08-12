#include "simulation/world.h"

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <cstddef>
#include <stdexcept>
#include <vector>

#include "random/random.h"
#include "simulation/animal.h"
#include "simulation/config.h"
#include "simulation/constants.h"
#include "simulation/food.h"

using cshorelark::simulation::animal;
using cshorelark::simulation::config;
using cshorelark::simulation::constants::k_two_pi;
using cshorelark::simulation::food;
using cshorelark::simulation::world;
using Catch::Matchers::WithinRel;

namespace {

// Test constants to avoid magic numbers
constexpr std::size_t k_test_num_foods = 5;
constexpr std::size_t k_test_num_animals = 3;
constexpr unsigned int k_test_rng_seed = 42;

auto create_test_config() -> config {
    config cfg;
    cfg.world.num_foods = k_test_num_foods;
    cfg.world.num_animals = k_test_num_animals;
    return cfg;
}

// Helper to create a deterministic RNG for testing
class test_rng : public cshorelark::random::random_generator {
public:
    test_rng() : cshorelark::random::random_generator(k_test_rng_seed) {}
};

}  // namespace

TEST_CASE("World - Basic functionality", "[world]") {
    SECTION("Can create random world") {
        auto cfg = create_test_config();
        test_rng rng;
        auto test_world = world::random(cfg, rng);
        
        REQUIRE(test_world.foods_count() == k_test_num_foods);
        REQUIRE(test_world.get_animals().size() == k_test_num_animals);
    }
    
    SECTION("Can access foods and animals") {
        auto cfg = create_test_config();
        test_rng rng;
        auto test_world = world::random(cfg, rng);
        
        const auto& foods = test_world.get_foods();
        const auto& animals = test_world.get_animals();
        
        REQUIRE(foods.size() == k_test_num_foods);
        REQUIRE(animals.size() == k_test_num_animals);
    }
}

TEST_CASE("World - Construction", "[world]") {
    SECTION("Can construct with animals and foods vectors") {
        test_rng rng;
        auto cfg = create_test_config();
        
        // Create some test animals and foods
        std::vector<animal> animals;
        std::vector<food> foods;
        
        for (std::size_t i = 0; i < k_test_num_animals; ++i) {
            animals.push_back(animal::random(cfg, rng));
        }
        
        for (std::size_t i = 0; i < k_test_num_foods; ++i) {
            foods.push_back(food::random(rng));
        }
        
        world test_world(std::move(animals), std::move(foods));
        
        REQUIRE(test_world.get_animals().size() == k_test_num_animals);
        REQUIRE(test_world.foods_count() == k_test_num_foods);
    }
}