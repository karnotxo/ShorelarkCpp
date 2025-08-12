#include "simulation/brain.h"

#include <algorithm>
#include <array>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <cstddef>
#include <vector>

#include "random/random.h"
#include "simulation/config.h"


using Catch::Matchers::WithinRel;
using cshorelark::simulation::brain;
using cshorelark::simulation::config;

namespace {

// Test constants to avoid magic numbers
constexpr std::size_t k_test_brain_neurons = 5;
constexpr std::size_t k_test_eye_cells = 13;
constexpr float k_test_value = 0.5F;
constexpr float k_deg_to_rad = 3.14159265F / 180.0F;

auto create_test_config() -> config {
    config cfg;
    cfg.brain_eye.num_neurons = k_test_brain_neurons;
    cfg.brain_eye.num_cells = k_test_eye_cells;
    return cfg;
}

// Helper to create a deterministic RNG for testing
class test_rng : public cshorelark::random::random_generator {
public:
    test_rng() : cshorelark::random::random_generator(42) {}  // Fixed seed for deterministic tests
};

}  // namespace

TEST_CASE("Brain - Basic initialization", "[brain]") {
    auto cfg = create_test_config();
    test_rng rng;
    auto brain_instance = brain::random(cfg, rng);

    SECTION("Network has correct input size") {
        std::vector<float> input(cfg.brain_eye.num_cells, k_test_value);
        auto result = brain_instance.propagate(input);

        REQUIRE(result.has_value());
        const auto& outputs = result.value();
        REQUIRE(outputs.size() == 2);  // speed and rotation

        // Both outputs should be non-zero for non-zero inputs with random weights
        REQUIRE(outputs[0] != 0.0F);
        REQUIRE(outputs[1] != 0.0F);
    }
}

TEST_CASE("Brain - Propagation", "[brain]") {
    auto cfg = create_test_config();
    test_rng rng1;
    auto brain_instance = brain::random(cfg, rng1);

    SECTION("Different inputs produce different outputs") {
        std::vector<float> input1(cfg.brain_eye.num_cells, 0.0F);
        std::vector<float> input2(cfg.brain_eye.num_cells, 1.0F);

        auto result1 = brain_instance.propagate(input1);
        auto result2 = brain_instance.propagate(input2);

        REQUIRE(result1.has_value());
        REQUIRE(result2.has_value());

        const auto& outputs1 = result1.value();
        const auto& outputs2 = result2.value();

        REQUIRE(outputs1.size() == 2);
        REQUIRE(outputs2.size() == 2);

        // Different inputs should produce different outputs
        REQUIRE((outputs1[0] != outputs2[0] || outputs1[1] != outputs2[1]));
    }

    SECTION("Same inputs produce same outputs") {
        std::vector<float> input(cfg.brain_eye.num_cells, k_test_value);

        auto result1 = brain_instance.propagate(input);
        auto result2 = brain_instance.propagate(input);

        REQUIRE(result1.has_value());
        REQUIRE(result2.has_value());

        const auto& outputs1 = result1.value();
        const auto& outputs2 = result2.value();

        REQUIRE_THAT(outputs1[0], WithinRel(outputs2[0]));
        REQUIRE_THAT(outputs1[1], WithinRel(outputs2[1]));
    }
}

TEST_CASE("Brain - Chromosome conversion", "[brain]") {
    auto cfg = create_test_config();
    test_rng rng1;
    auto brain_instance = brain::random(cfg, rng1);

    SECTION("Can convert to and from chromosome") {
        auto chromosome = brain_instance.as_chromosome();
        auto new_brain_result = brain::from_chromosome(cfg, chromosome);

        REQUIRE(new_brain_result.has_value());
        auto new_brain = std::move(new_brain_result.value());

        // Test with same input
        std::vector<float> input(cfg.brain_eye.num_cells, k_test_value);
        auto result1 = brain_instance.propagate(input);
        auto result2 = new_brain.propagate(input);

        REQUIRE(result1.has_value());
        REQUIRE(result2.has_value());

        const auto& outputs1 = result1.value();
        const auto& outputs2 = result2.value();

        REQUIRE_THAT(outputs1[0], WithinRel(outputs2[0]));
        REQUIRE_THAT(outputs1[1], WithinRel(outputs2[1]));
    }

    SECTION("Different chromosomes produce different behaviors") {
        auto chromosome1 = brain_instance.as_chromosome();
        test_rng rng2;
        auto brain2 = brain::random(cfg, rng2);
        auto chromosome2 = brain2.as_chromosome();

        // Compare chromosome genes using standard vector comparison
        const auto& genes1 = chromosome1.genes();
        const auto& genes2 = chromosome2.genes();
        bool chromosomes_equal =
            std::equal(genes1.begin(), genes1.end(), genes2.begin(), genes2.end());
        REQUIRE(!chromosomes_equal);

        std::vector<float> input(cfg.brain_eye.num_cells, k_test_value);
        auto brain1_result = brain::from_chromosome(cfg, chromosome1);
        auto brain2_result = brain::from_chromosome(cfg, chromosome2);

        REQUIRE(brain1_result.has_value());
        REQUIRE(brain2_result.has_value());

        auto result1 = brain1_result.value().propagate(input);
        auto result2 = brain2_result.value().propagate(input);

        REQUIRE(result1.has_value());
        REQUIRE(result2.has_value());

        const auto& outputs1 = result1.value();
        const auto& outputs2 = result2.value();

        // With different weights, outputs should differ
        bool outputs_differ = (outputs1[0] != outputs2[0]) || (outputs1[1] != outputs2[1]);
        REQUIRE(outputs_differ);
    }
}

TEST_CASE("Brain - Output ranges", "[brain]") {
    auto cfg = create_test_config();
    test_rng rng;
    auto brain_instance = brain::random(cfg, rng);

    SECTION("Speed and rotation are within expected ranges") {
        std::vector<float> input(cfg.brain_eye.num_cells, k_test_value);
        auto result = brain_instance.propagate(input);

        REQUIRE(result.has_value());
        const auto& outputs = result.value();
        REQUIRE(outputs.size() == 2);

        // Check that outputs are within the acceleration limits set in config
        // The brain clamps speed to [-speed_accel, speed_accel] and rotation to [-rotation_accel,
        // rotation_accel]
        REQUIRE(outputs[0] >= -cfg.sim.speed_accel);
        REQUIRE(outputs[0] <= cfg.sim.speed_accel);

        // Convert rotation limit from degrees to radians for comparison
        const float rotation_limit_rad = cfg.sim.rotation_accel_deg * k_deg_to_rad;
        REQUIRE(outputs[1] >= -rotation_limit_rad);
        REQUIRE(outputs[1] <= rotation_limit_rad);
    }
}