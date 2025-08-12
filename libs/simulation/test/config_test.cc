#include "simulation/config.h"  // NOLINT(build/include_subdir)

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

using Catch::Matchers::WithinRel;
using cshorelark::simulation::config;

// Test constants to avoid magic numbers
namespace {
constexpr int k_default_brain_neurons = 9;
constexpr int k_default_eye_cells = 9;
constexpr float k_default_fov_range = 0.25F;
constexpr float k_default_fov_angle_deg = 225.0F;  // PI + PI/4 in degrees
constexpr float k_default_food_size = 0.01F;
constexpr int k_default_animals = 40;
constexpr int k_default_foods = 60;
constexpr float k_default_mutation_chance = 0.01F;
constexpr float k_default_mutation_coeff = 0.3F;
constexpr float k_default_speed_min = 0.001F;
constexpr float k_default_speed_max = 0.005F;
constexpr float k_default_speed_accel = 0.2F;
constexpr float k_default_rotation_accel_deg = 90.0F;  // PI/2 in degrees
constexpr int k_default_generation_length = 2500;

// Test value constants
constexpr int k_test_neurons_12 = 12;
constexpr int k_test_neurons_15 = 15;
constexpr float k_test_fov_range_half = 0.5F;
constexpr float k_test_fov_angle_180 = 180.0F;  // PI in degrees
constexpr float k_test_food_size = 0.02F;
constexpr int k_test_animals_80 = 80;
constexpr int k_test_animals_100 = 100;
constexpr int k_test_foods_120 = 120;
constexpr float k_test_mutation_chance = 0.05F;
constexpr float k_test_mutation_coeff = 0.5F;
constexpr float k_test_speed_min = 0.002F;
constexpr float k_test_speed_max = 0.01F;
constexpr float k_test_speed_accel = 0.3F;
constexpr int k_test_generation_length = 5000;

// Edge case constants
constexpr float k_zero_value = 0.0F;
constexpr float k_one_value = 1.0F;
constexpr float k_full_circle_deg = 360.0F;  // 2*PI in degrees
constexpr int k_min_value_1 = 1;
constexpr int k_min_animals = 2;
constexpr int k_min_foods = 1;
constexpr float k_min_size = 0.001F;
}  // namespace

TEST_CASE("Config default values match Rust implementation", "[config]") {
    config cfg;

    // Brain and Eye configuration
    REQUIRE(cfg.brain_eye.num_neurons == k_default_brain_neurons);
    REQUIRE_THAT(cfg.brain_eye.fov_range, WithinRel(k_default_fov_range));
    REQUIRE_THAT(cfg.brain_eye.fov_angle_deg, WithinRel(k_default_fov_angle_deg));
    REQUIRE(cfg.brain_eye.num_cells == k_default_eye_cells);

    // World configuration (includes food size)
    REQUIRE_THAT(cfg.world.food_size, WithinRel(k_default_food_size));
    REQUIRE(cfg.world.num_animals == k_default_animals);
    REQUIRE(cfg.world.num_foods == k_default_foods);

    // Genetic algorithm configuration
    REQUIRE(cfg.genetic.reverse == false);  // 0 in Rust becomes false in C++
    REQUIRE_THAT(cfg.genetic.mutation_chance, WithinRel(k_default_mutation_chance));
    REQUIRE_THAT(cfg.genetic.mutation_coeff, WithinRel(k_default_mutation_coeff));

    // Simulation physics configuration
    REQUIRE_THAT(cfg.sim.speed_min, WithinRel(k_default_speed_min));
    REQUIRE_THAT(cfg.sim.speed_max, WithinRel(k_default_speed_max));
    REQUIRE_THAT(cfg.sim.speed_accel, WithinRel(k_default_speed_accel));
    REQUIRE_THAT(cfg.sim.rotation_accel_deg, WithinRel(k_default_rotation_accel_deg));
    REQUIRE(cfg.sim.generation_length == k_default_generation_length);
}

TEST_CASE("Config values can be modified", "[config]") {
    config cfg;

    SECTION("Brain and eye configuration") {
        cfg.brain_eye.num_neurons = k_test_neurons_12;
        REQUIRE(cfg.brain_eye.num_neurons == k_test_neurons_12);

        cfg.brain_eye.fov_range = k_test_fov_range_half;
        REQUIRE_THAT(cfg.brain_eye.fov_range, WithinRel(k_test_fov_range_half));

        cfg.brain_eye.fov_angle_deg = k_test_fov_angle_180;
        REQUIRE_THAT(cfg.brain_eye.fov_angle_deg, WithinRel(k_test_fov_angle_180));

        cfg.brain_eye.num_cells = k_test_neurons_12;
        REQUIRE(cfg.brain_eye.num_cells == k_test_neurons_12);
    }

    SECTION("World configuration") {
        cfg.world.food_size = k_test_food_size;
        REQUIRE_THAT(cfg.world.food_size, WithinRel(k_test_food_size));

        cfg.world.num_animals = k_test_animals_80;
        REQUIRE(cfg.world.num_animals == k_test_animals_80);

        cfg.world.num_foods = k_test_foods_120;
        REQUIRE(cfg.world.num_foods == k_test_foods_120);
    }

    SECTION("Genetic algorithm configuration") {
        cfg.genetic.reverse = true;  // 2 in Rust becomes true in C++
        REQUIRE(cfg.genetic.reverse == true);

        cfg.genetic.mutation_chance = k_test_mutation_chance;
        REQUIRE_THAT(cfg.genetic.mutation_chance, WithinRel(k_test_mutation_chance));

        cfg.genetic.mutation_coeff = k_test_mutation_coeff;
        REQUIRE_THAT(cfg.genetic.mutation_coeff, WithinRel(k_test_mutation_coeff));
    }

    SECTION("Simulation physics configuration") {
        cfg.sim.speed_min = k_test_speed_min;
        REQUIRE_THAT(cfg.sim.speed_min, WithinRel(k_test_speed_min));

        cfg.sim.speed_max = k_test_speed_max;
        REQUIRE_THAT(cfg.sim.speed_max, WithinRel(k_test_speed_max));

        cfg.sim.speed_accel = k_test_speed_accel;
        REQUIRE_THAT(cfg.sim.speed_accel, WithinRel(k_test_speed_accel));

        cfg.sim.rotation_accel_deg = k_test_fov_angle_180;
        REQUIRE_THAT(cfg.sim.rotation_accel_deg, WithinRel(k_test_fov_angle_180));

        cfg.sim.generation_length = k_test_generation_length;
        REQUIRE(cfg.sim.generation_length == k_test_generation_length);
    }
}

TEST_CASE("Config copy construction and assignment work correctly", "[config]") {
    config original;
    original.brain_eye.num_neurons = k_test_neurons_15;
    original.brain_eye.fov_range = k_test_fov_range_half;
    original.world.num_animals = k_test_animals_100;

    SECTION("Copy construction") {
        config copy(original);
        REQUIRE(copy.brain_eye.num_neurons == k_test_neurons_15);
        REQUIRE_THAT(copy.brain_eye.fov_range, WithinRel(k_test_fov_range_half));
        REQUIRE(copy.world.num_animals == k_test_animals_100);
    }

    SECTION("Copy assignment") {
        config copy;
        copy = original;
        REQUIRE(copy.brain_eye.num_neurons == k_test_neurons_15);
        REQUIRE_THAT(copy.brain_eye.fov_range, WithinRel(k_test_fov_range_half));
        REQUIRE(copy.world.num_animals == k_test_animals_100);
    }
}

TEST_CASE("Config move construction and assignment work correctly", "[config]") {
    config original;
    original.brain_eye.num_neurons = k_test_neurons_15;
    original.brain_eye.fov_range = k_test_fov_range_half;
    original.world.num_animals = k_test_animals_100;

    SECTION("Move construction") {
        // Note: For trivially copyable types, move construction is equivalent to copy
        config moved(original);  // Removed std::move for trivially copyable type
        REQUIRE(moved.brain_eye.num_neurons == k_test_neurons_15);
        REQUIRE_THAT(moved.brain_eye.fov_range, WithinRel(k_test_fov_range_half));
        REQUIRE(moved.world.num_animals == k_test_animals_100);
    }

    SECTION("Move assignment") {
        // Note: For trivially copyable types, move assignment is equivalent to copy
        config moved;
        moved = original;  // Removed std::move for trivially copyable type
        REQUIRE(moved.brain_eye.num_neurons == k_test_neurons_15);
        REQUIRE_THAT(moved.brain_eye.fov_range, WithinRel(k_test_fov_range_half));
        REQUIRE(moved.world.num_animals == k_test_animals_100);
    }
}

TEST_CASE("Config edge cases and constraints", "[config]") {
    config cfg;

    SECTION("Brain and eye configuration constraints") {
        // FOV range should be positive
        cfg.brain_eye.fov_range = k_zero_value;
        REQUIRE_THAT(cfg.brain_eye.fov_range, WithinRel(k_zero_value));

        // FOV angle should be positive and can be up to 360 degrees
        cfg.brain_eye.fov_angle_deg = k_zero_value;
        REQUIRE_THAT(cfg.brain_eye.fov_angle_deg, WithinRel(k_zero_value));
        cfg.brain_eye.fov_angle_deg = k_full_circle_deg;
        REQUIRE_THAT(cfg.brain_eye.fov_angle_deg, WithinRel(k_full_circle_deg));

        // Eye cells should be positive
        cfg.brain_eye.num_cells = k_min_value_1;
        REQUIRE(cfg.brain_eye.num_cells == k_min_value_1);

        // Brain neurons should be positive
        cfg.brain_eye.num_neurons = k_min_value_1;
        REQUIRE(cfg.brain_eye.num_neurons == k_min_value_1);
    }

    SECTION("Genetic algorithm constraints") {
        // Mutation chance should be between 0 and 1
        cfg.genetic.mutation_chance = k_zero_value;
        REQUIRE_THAT(cfg.genetic.mutation_chance, WithinRel(k_zero_value));
        cfg.genetic.mutation_chance = k_one_value;
        REQUIRE_THAT(cfg.genetic.mutation_chance, WithinRel(k_one_value));

        // Mutation coefficient should be positive
        cfg.genetic.mutation_coeff = k_zero_value;
        REQUIRE_THAT(cfg.genetic.mutation_coeff, WithinRel(k_zero_value));

        // Reverse can be true or false
        cfg.genetic.reverse = true;
        REQUIRE(cfg.genetic.reverse == true);
        cfg.genetic.reverse = false;
        REQUIRE(cfg.genetic.reverse == false);
    }

    SECTION("Simulation physics constraints") {
        // Speed min/max relationship
        cfg.sim.speed_min = k_default_speed_min;
        cfg.sim.speed_max = k_test_speed_min;
        REQUIRE_THAT(cfg.sim.speed_min, WithinRel(k_default_speed_min));
        REQUIRE_THAT(cfg.sim.speed_max, WithinRel(k_test_speed_min));
        REQUIRE(cfg.sim.speed_min <= cfg.sim.speed_max);

        // Speed and rotation acceleration should be positive
        cfg.sim.speed_accel = k_zero_value;
        REQUIRE_THAT(cfg.sim.speed_accel, WithinRel(k_zero_value));
        cfg.sim.rotation_accel_deg = k_zero_value;
        REQUIRE_THAT(cfg.sim.rotation_accel_deg, WithinRel(k_zero_value));

        // Generation length should be positive
        cfg.sim.generation_length = k_min_value_1;
        REQUIRE(cfg.sim.generation_length == k_min_value_1);
    }

    SECTION("World configuration constraints") {
        // Should allow minimum viable population
        cfg.world.num_animals = k_min_animals;  // Minimum for reproduction
        cfg.world.num_foods = k_min_foods;      // Minimum for survival
        REQUIRE(cfg.world.num_animals == k_min_animals);
        REQUIRE(cfg.world.num_foods == k_min_foods);

        // Food and bird size should be positive
        cfg.world.food_size = k_min_size;
        cfg.world.bird_size = k_min_size;
        REQUIRE_THAT(cfg.world.food_size, WithinRel(k_min_size));
        REQUIRE_THAT(cfg.world.bird_size, WithinRel(k_min_size));
    }
}