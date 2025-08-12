#include "simulation/animal.h"

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <cmath>
#include <vector>

#include "random/random.h"
#include "simulation/config.h"
#include "simulation/constants.h"
#include "simulation/food.h"

using Catch::Matchers::WithinAbs;
using Catch::Matchers::WithinRel;
using cshorelark::simulation::animal;
using cshorelark::simulation::config;
using cshorelark::simulation::food;
using cshorelark::simulation::vector2d;

namespace {
// Helper to create a deterministic RNG for testing
class test_rng {
public:
    static auto next_float() -> float {
        static float values[] = {0.5F, 0.5F, 0.5F, 0.5F};
        static std::size_t index = 0;
        return values[index++ % 4];
    }
};

// Helper to create a test config
auto create_test_config() -> config {
    config cfg;
    // Set brain and eye config
    cfg.brain_eye.fov_range = 0.25F;
    cfg.brain_eye.fov_angle_deg = 225.0F;
    cfg.brain_eye.num_cells = 9;
    cfg.brain_eye.num_neurons = 9;

    // Set simulation config
    cfg.sim.speed_min = 0.001F;
    cfg.sim.speed_max = 0.005F;
    cfg.sim.speed_accel = 0.2F;
    cfg.sim.rotation_accel_deg = 90.0F;
    cfg.sim.generation_length = 2500;

    // Set world config
    cfg.world.num_foods = 60;
    cfg.world.num_animals = 40;
    cfg.world.food_size = 0.01F;
    cfg.world.bird_size = 0.015F;

    // Set genetic config
    cfg.genetic.mutation_chance = 0.01F;
    cfg.genetic.mutation_coeff = 0.3F;
    cfg.genetic.reverse = false;

    return cfg;
}

// Helper to compare chromosomes with epsilon
bool compare_chromosomes(const cshorelark::genetic::chromosome& a,
                         const cshorelark::genetic::chromosome& b, float epsilon = 0.0001F) {
    if (a.size() != b.size()) {
        return false;
    }
    for (std::size_t i = 0; i < a.size(); ++i) {
        if (std::abs(a[i] - b[i]) > epsilon) {
            return false;
        }
    }
    return true;
}

}  // namespace

TEST_CASE("animal - Random creation", "[animal]") {
    auto cfg = create_test_config();
    auto random = cshorelark::random::random_generator{};
    auto test_animal = animal::random(cfg, random);

    SECTION("Initial position is within bounds") {
        const auto& pos = test_animal.position();
        REQUIRE(pos.x() >= 0.0F);
        REQUIRE(pos.x() <= 1.0F);
        REQUIRE(pos.y() >= 0.0F);
        REQUIRE(pos.y() <= 1.0F);
    }

    SECTION("Initial rotation is normalized") {
        float angle = test_animal.rotation();
        REQUIRE(angle >= 0.0F);
        REQUIRE(angle <= 2.0F * cshorelark::simulation::k_pi);
    }

    SECTION("Initial speed matches config") {
        REQUIRE_THAT(test_animal.speed(), WithinRel(cfg.sim.speed_max));
    }

    SECTION("Initial food eaten is zero") { REQUIRE(test_animal.food_eaten() == 0); }
}

TEST_CASE("animal - Chromosome conversion", "[animal]") {
    auto cfg = create_test_config();
    auto random = cshorelark::random::random_generator{};
    auto original = animal::random(cfg, random);

    SECTION("Can convert to and from chromosome") {
        auto chromosome = original.as_chromosome();
        auto result = animal::from_chromosome(cfg, random, chromosome);
        REQUIRE(result.has_value());

        auto& new_animal = result.value();
        auto new_chromosome = new_animal.as_chromosome();

        // Brain weights should be preserved using epsilon comparison
        REQUIRE(compare_chromosomes(chromosome, new_chromosome));
    }

    SECTION("Food eaten is preserved in chromosome conversion") {
        original.increment_food_eaten();
        original.increment_food_eaten();  // Now has eaten 2 food items

        auto chromosome = original.as_chromosome();
        auto result = animal::from_chromosome(cfg, random, chromosome);
        REQUIRE(result.has_value());

        auto& new_animal = result.value();
        REQUIRE(new_animal.food_eaten() == 2);
    }
}

TEST_CASE("animal - Brain processing", "[animal]") {
    auto cfg = create_test_config();
    auto random = cshorelark::random::random_generator{};
    auto test_animal = animal::random(cfg, random);

    std::vector<food> foods;
    foods.emplace_back(vector2d{0.1F, 0.1F});
    foods.emplace_back(vector2d{0.9F, 0.9F});

    SECTION("Updates rotation and speed based on vision") {
        const auto initial_rot = test_animal.rotation();
        const auto initial_speed = test_animal.speed();

        test_animal.process_brain(cfg, foods);

        // Either rotation or speed should change
        REQUIRE((test_animal.rotation() != initial_rot || test_animal.speed() != initial_speed));

        // Speed should be within bounds
        REQUIRE(test_animal.speed() >= cfg.sim.speed_min);
        REQUIRE(test_animal.speed() <= cfg.sim.speed_max);
    }

    SECTION("Vision is updated based on food positions") {
        const auto& vision = test_animal.vision();
        REQUIRE(vision.size() == cfg.brain_eye.num_cells);

        test_animal.process_brain(cfg, foods);
        const auto& new_vision = test_animal.vision();
        REQUIRE(new_vision.size() == cfg.brain_eye.num_cells);

        // Vision should change after processing
        REQUIRE(vision != new_vision);
    }
}

TEST_CASE("animal - Movement processing", "[animal]") {
    auto cfg = create_test_config();
    auto random = cshorelark::random::random_generator{};
    auto test_animal = animal::random(cfg, random);

    SECTION("Position is updated based on speed and rotation") {
        const auto initial_pos = test_animal.position();
        test_animal.process_movement();

        // Position should change
        REQUIRE(test_animal.position() != initial_pos);
    }

    SECTION("Position is wrapped to world bounds") {
        // Position at edge
        test_animal.set_position(vector2d{0.98F, 0.98F});
        test_animal.set_rotation(cshorelark::simulation::constants::k_pi_over4);  // 45 degrees
        test_animal.set_speed(0.2F);                                              // Fast speed

        test_animal.process_movement();

        // Position should be wrapped
        const auto& pos = test_animal.position();
        REQUIRE(pos.x() >= 0.0F);
        REQUIRE(pos.x() <= 1.0F);
        REQUIRE(pos.y() >= 0.0F);
        REQUIRE(pos.y() <= 1.0F);
    }
}

TEST_CASE("animal - Food eaten tracking", "[animal]") {
    auto cfg = create_test_config();
    auto random = cshorelark::random::random_generator{};
    auto test_animal = animal::random(cfg, random);

    SECTION("Food eaten counter increments") {
        REQUIRE(test_animal.food_eaten() == 0);
        test_animal.increment_food_eaten();
        REQUIRE(test_animal.food_eaten() == 1);
        test_animal.increment_food_eaten();
        REQUIRE(test_animal.food_eaten() == 2);
    }
}