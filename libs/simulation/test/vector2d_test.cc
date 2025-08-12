#include "simulation/vector2d.h"

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

namespace cshorelark::simulation {
namespace {

// Test constants to avoid magic numbers
constexpr float k_test_x1 = 1.0F;
constexpr float k_test_x2 = 2.0F;
constexpr float k_test_x3 = 3.0F;
constexpr float k_test_x4 = 4.0F;
constexpr float k_test_y2 = 2.0F;
constexpr float k_test_y3 = 3.0F;
constexpr float k_test_y4 = 4.0F;
constexpr float k_test_y5 = 5.0F;
constexpr float k_test_y6 = 6.0F;
constexpr float k_test_scalar = 2.0F;
constexpr float k_test_half = 0.5F;
constexpr float k_test_expected_length = 5.0F;
constexpr float k_test_expected_length_squared = 25.0F;
constexpr float k_test_expected_normalized_x = 0.6F;
constexpr float k_test_expected_normalized_y = 0.8F;
constexpr float k_test_unit_length = 1.0F;

TEST_CASE("Vector2D - Default Construction", "[vector2d]") {
    const vector2d test_vec;
    CHECK(test_vec.x() == 0.0F);
    CHECK(test_vec.y() == 0.0F);
}

TEST_CASE("Vector2D - Construction with Values", "[vector2d]") {
    const vector2d test_vec(k_test_x3, k_test_y4);
    CHECK(test_vec.x() == k_test_x3);
    CHECK(test_vec.y() == k_test_y4);
}

TEST_CASE("Vector2D - Addition", "[vector2d]") {
    const vector2d vec_first(k_test_x1, k_test_y2);
    const vector2d vec_second(k_test_x3, k_test_y4);
    const vector2d result = vec_first + vec_second;
    CHECK(result.x() == k_test_y4);
    CHECK(result.y() == k_test_y6);
}

TEST_CASE("Vector2D - Subtraction", "[vector2d]") {
    const vector2d vec_first(k_test_x3, k_test_y4);
    const vector2d vec_second(k_test_x1, k_test_y2);
    const vector2d result = vec_first - vec_second;
    CHECK(result.x() == k_test_x2);
    CHECK(result.y() == k_test_y2);
}

TEST_CASE("Vector2D - Scalar Multiplication", "[vector2d]") {
    const vector2d test_vec(k_test_x2, k_test_y3);

    SECTION("Vector * Scalar") {
        const vector2d result = test_vec * k_test_scalar;
        CHECK(result.x() == k_test_y4);
        CHECK(result.y() == k_test_y6);
    }

    SECTION("Scalar * Vector") {
        const vector2d result = k_test_scalar * test_vec;
        CHECK(result.x() == k_test_y4);
        CHECK(result.y() == k_test_y6);
    }
}

TEST_CASE("Vector2D - Length", "[vector2d]") {
    const vector2d test_vec(k_test_x3, k_test_y4);
    REQUIRE_THAT(test_vec.length(), Catch::Matchers::WithinRel(k_test_expected_length));
    CHECK(test_vec.length_squared() == k_test_expected_length_squared);
}

TEST_CASE("Vector2D - Normalization", "[vector2d]") {
    SECTION("Non-zero vector") {
        vector2d test_vec(k_test_x3, k_test_y4);
        test_vec.normalize();
        REQUIRE_THAT(test_vec.x(), Catch::Matchers::WithinRel(k_test_expected_normalized_x));
        REQUIRE_THAT(test_vec.y(), Catch::Matchers::WithinRel(k_test_expected_normalized_y));
        REQUIRE_THAT(test_vec.length(), Catch::Matchers::WithinRel(k_test_unit_length));
    }

    SECTION("Zero vector") {
        vector2d test_vec(0.0F, 0.0F);
        test_vec.normalize();
        CHECK(test_vec.x() == 0.0F);
        CHECK(test_vec.y() == 0.0F);
    }
}

TEST_CASE("Vector2D - Compound Assignment", "[vector2d]") {
    vector2d test_vec(k_test_x1, k_test_y2);

    SECTION("Addition") {
        test_vec += vector2d(k_test_x2, k_test_y3);
        CHECK(test_vec.x() == k_test_x3);
        CHECK(test_vec.y() == k_test_y5);
    }

    SECTION("Subtraction") {
        test_vec -= vector2d(k_test_half, k_test_x1);
        CHECK(test_vec.x() == k_test_half);
        CHECK(test_vec.y() == k_test_x1);
    }

    SECTION("Multiplication") {
        test_vec *= k_test_scalar;
        CHECK(test_vec.x() == k_test_x2);
        CHECK(test_vec.y() == k_test_y4);
    }
}

}  // namespace
}  // namespace cshorelark::simulation