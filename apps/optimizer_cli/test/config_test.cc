#include <catch2/catch_test_macros.hpp>
#include <optimizer/config.hpp>

using namespace shorelark;

TEST_CASE("Config defaults are valid", "[config]") {
    Config config;
    CHECK(config.is_valid());
}

TEST_CASE("Config validates brain neurons", "[config]") {
    Config config;

    config.brain_neurons = 0;
    CHECK_FALSE(config.is_valid());

    config.brain_neurons = 1;
    CHECK(config.is_valid());

    config.brain_neurons = 255;
    CHECK(config.is_valid());
}

TEST_CASE("Config validates eye FOV range", "[config]") {
    Config config;

    config.eye_fov_range = 0.0f;
    CHECK_FALSE(config.is_valid());

    config.eye_fov_range = -1.0f;
    CHECK_FALSE(config.is_valid());

    config.eye_fov_range = 5.0f;
    CHECK(config.is_valid());

    config.eye_fov_range = 10.1f;
    CHECK_FALSE(config.is_valid());
}

TEST_CASE("Config validates eye FOV angle", "[config]") {
    Config config;

    config.eye_fov_angle = 0.0f;
    CHECK_FALSE(config.is_valid());

    config.eye_fov_angle = -1.0f;
    CHECK_FALSE(config.is_valid());

    config.eye_fov_angle = 3.14f;
    CHECK(config.is_valid());

    config.eye_fov_angle = 7.0f;
    CHECK_FALSE(config.is_valid());
}

TEST_CASE("Config validates eye cells", "[config]") {
    Config config;

    config.eye_cells = 0;
    CHECK_FALSE(config.is_valid());

    config.eye_cells = 1;
    CHECK(config.is_valid());

    config.eye_cells = 100;
    CHECK(config.is_valid());
}

TEST_CASE("Config validates mutation chance", "[config]") {
    Config config;

    config.ga_mutation_chance = -0.1f;
    CHECK_FALSE(config.is_valid());

    config.ga_mutation_chance = 0.0f;
    CHECK(config.is_valid());

    config.ga_mutation_chance = 0.5f;
    CHECK(config.is_valid());

    config.ga_mutation_chance = 1.0f;
    CHECK(config.is_valid());

    config.ga_mutation_chance = 1.1f;
    CHECK_FALSE(config.is_valid());
}

TEST_CASE("Config validates mutation coefficient", "[config]") {
    Config config;

    config.ga_mutation_coeff = 0.0f;
    CHECK_FALSE(config.is_valid());

    config.ga_mutation_coeff = -1.0f;
    CHECK_FALSE(config.is_valid());

    config.ga_mutation_coeff = 0.5f;
    CHECK(config.is_valid());
}

TEST_CASE("Config allows all valid combinations", "[config]") {
    Config config;

    // Test some valid combinations
    config.brain_neurons = 20;
    config.eye_fov_range = 5.0f;
    config.eye_fov_angle = 2.0f;
    config.eye_cells = 15;
    config.ga_mutation_chance = 0.05f;
    config.ga_mutation_coeff = 0.75f;

    CHECK(config.is_valid());
}