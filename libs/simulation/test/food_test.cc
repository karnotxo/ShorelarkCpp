#include "simulation/food.h"

#include <array>
#include <cstddef>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "random/random.h"
#include "simulation/vector2d.h"

using cshorelark::simulation::food;
using cshorelark::simulation::vector2d;
using Catch::Matchers::WithinRel;

namespace {

// Test constants to avoid magic numbers
constexpr float k_test_x_pos = 0.3F;
constexpr float k_test_y_pos = 0.7F;
constexpr float k_test_x_pos2 = 0.4F;
constexpr float k_test_y_pos2 = 0.6F;
constexpr float k_expected_random_pos = 0.5F;

// Helper to create a deterministic RNG for testing
class test_rng : public cshorelark::random::random_generator {
public:
    test_rng() : cshorelark::random::random_generator(42) {}  // Fixed seed for deterministic tests
};

}  // namespace

TEST_CASE("Food - Basic initialization", "[food]") {
    SECTION("Can be created at specific position") {
        const food foodie(vector2d(k_test_x_pos, k_test_y_pos));
        REQUIRE_THAT(foodie.position().x(), WithinRel(k_test_x_pos));
        REQUIRE_THAT(foodie.position().y(), WithinRel(k_test_y_pos));
    }
}

TEST_CASE("Food - Random generation", "[food]") {
    SECTION("Random food is within bounds") {
        test_rng rng;
        auto food_item = food::random(rng);

        // Position should be between 0 and 1
        REQUIRE(food_item.position().x() >= 0.0F);
        REQUIRE(food_item.position().x() <= 1.0F);
        REQUIRE(food_item.position().y() >= 0.0F);
        REQUIRE(food_item.position().y() <= 1.0F);
    }

    SECTION("Multiple random foods have consistent positions") {
        test_rng rng1;
        test_rng rng2;
        auto food1 = food::random(rng1);
        auto food2 = food::random(rng2);

        // With our deterministic test RNG using same seed, positions should be identical
        REQUIRE_THAT(food1.position().x(), WithinRel(food2.position().x()));
        REQUIRE_THAT(food1.position().y(), WithinRel(food2.position().y()));
    }
}

TEST_CASE("Food - Position access", "[food]") {
    SECTION("Position is immutable") {
        const food foodie(vector2d(k_test_x_pos2, k_test_y_pos2));
        auto pos = foodie.position();
        REQUIRE_THAT(pos.x(), WithinRel(k_test_x_pos2));
        REQUIRE_THAT(pos.y(), WithinRel(k_test_y_pos2));
    }
}