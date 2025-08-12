#include <random/random.h>

#include <array>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <cmath>
#include <numeric>
#include <vector>


using namespace cshorelark::random;
using Catch::Matchers::WithinRel;

TEST_CASE("RandomGenerator construction", "[random]") {
    SECTION("default constructor uses random seed") {
        random_generator gen1;
        random_generator gen2;

        // Generate several values from each generator
        std::array<float, 10> values1{};
        std::array<float, 10> values2{};

        for (size_t i = 0; i < values1.size(); ++i) {
            values1[i] = gen1.generate_weight();
            values2[i] = gen2.generate_weight();
        }

        // Check that the two generators produce different values
        // because they should have different seeds
        bool different_values = false;
        for (size_t i = 0; i < values1.size(); ++i) {
            if (values1[i] != values2[i]) {
                different_values = true;
                break;
            }
        }

        CHECK(different_values);
    }

    SECTION("seeded constructor produces reproducible results") {
        constexpr uint64_t seed = 42;
        random_generator gen1(seed);
        random_generator gen2(seed);

        // Both generators should produce the same sequence
        for (int i = 0; i < 10; ++i) {
            REQUIRE(gen1.generate_weight() == gen2.generate_weight());
        }
    }
}

TEST_CASE("RandomGenerator methods", "[random]") {
    // Use a fixed seed for reproducible tests
    random_generator gen(42);

    SECTION("generate_weight produces values in [-1, 1]") {
        const size_t iterations = 1000;
        float min_value = 1.0F;
        float max_value = -1.0F;

        for (size_t i = 0; i < iterations; ++i) {
            const float value = gen.generate_weight();

            // Track min/max values
            if (value < min_value)
                min_value = value;
            if (value > max_value)
                max_value = value;

            REQUIRE(value >= -1.0F);
            REQUIRE(value <= 1.0F);
        }

        // With enough iterations, we should get close to the bounds
        CHECK(min_value < -0.9F);
        CHECK(max_value > 0.9F);
    }

    SECTION("generate_position produces values in [0, 1]") {
        const size_t iterations = 1000;
        float min_value = 1.0F;
        float max_value = 0.0F;

        for (size_t i = 0; i < iterations; ++i) {
            const float value = gen.generate_position();

            // Track min/max values
            if (value < min_value)
                min_value = value;
            if (value > max_value)
                max_value = value;

            REQUIRE(value >= 0.0F);
            REQUIRE(value <= 1.0F);
        }

        // With enough iterations, we should get close to the bounds
        CHECK(min_value < 0.1F);
        CHECK(max_value > 0.9F);
    }

    SECTION("generate_rotation produces values in [-π, π)") {
        constexpr float pi = 3.14159265358979323846F;
        const size_t iterations = 1000;
        float min_value = pi;
        float max_value = -pi;

        for (size_t i = 0; i < iterations; ++i) {
            const float value = gen.generate_rotation();

            // Track min/max values
            if (value < min_value)
                min_value = value;
            if (value > max_value)
                max_value = value;

            REQUIRE(value >= -pi);
            REQUIRE(value <= pi);
        }

        // With enough iterations, we should get close to the bounds
        CHECK(min_value < -0.9F * pi);
        CHECK(max_value > 0.9F * pi);
    }

    SECTION("multiple calls advance the state") {
        random_generator gen1(42);
        random_generator gen2(42);

        // Advance gen1 but not gen2
        gen1.generate_weight();
        gen2.generate_weight();

        // Both generators should now be in the same state
        REQUIRE(gen1.generate_weight() == gen2.generate_weight());
    }

    SECTION("get_engine provides access to underlying engine") {
        random_generator gen(42);

        // Store initial state
        auto initial_state = gen.get_engine();

        // Generate a value directly using the engine
        std::uniform_real_distribution<float> dist(-1.0F, 1.0F);
        const float direct_value = dist(gen.get_engine());

        // Engine state should have changed
        CHECK(gen.get_engine() != initial_state);

        // Resetting the seed should reset the state
        gen.seed(42);

        // Generate a value using the generator method
        const float weight = gen.generate_weight();

        // Should match the direct value
        CHECK_THAT(weight, WithinRel(direct_value));
    }
}