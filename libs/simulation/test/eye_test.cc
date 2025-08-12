#include "simulation/eye.h"

#include <algorithm>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <cstddef>
#include <string>
#include <vector>

#include "simulation/config.h"
#include "simulation/constants.h"
#include "simulation/food.h"
#include "simulation/vector2d.h"


using Catch::Matchers::WithinRel;
using cshorelark::simulation::brain_eye_config;
using cshorelark::simulation::config;
using cshorelark::simulation::eye;
using cshorelark::simulation::food;
using cshorelark::simulation::vector2d;

namespace {

// Test constants to avoid magic numbers
constexpr std::size_t k_test_eye_cells = 13;
constexpr float k_test_fov_range = 1.0F;

auto create_test_config() -> brain_eye_config {
    brain_eye_config cfg;
    cfg.num_cells = k_test_eye_cells;
    cfg.fov_angle_deg = 90.0F;  // Default 90 degrees
    cfg.fov_range = k_test_fov_range;
    return cfg;
}

// Helper to create food at specific position
auto create_food(float x_pos, float y_pos) -> food { return food(vector2d(x_pos, y_pos)); }

// Helper to visualize eye cells (similar to Rust test)
auto visualize_vision(const std::vector<float>& vision) -> std::string {
    std::string result;
    for (float cell : vision) {
        if (cell >= 0.7F) {
            result += "#";
        } else if (cell >= 0.3F) {
            result += "+";
        } else if (cell > 0.0F) {
            result += ".";
        } else {
            result += " ";
        }
    }
    return result;
}

// Test case helper struct similar to Rust implementation
struct test_case {
    std::vector<food> foods;
    float fov_angle;
    float fov_range;
    float x;
    float y;
    float rot;
    std::string expected;

    void run() const {
        brain_eye_config cfg = create_test_config();
        cfg.fov_angle_deg =
            fov_angle * 180.0F / cshorelark::simulation::constants::k_pi;  // Convert to degrees
        cfg.fov_range = fov_range;

        eye eye_instance(cfg);
        auto vision = eye_instance.process_vision(vector2d(x, y), rot, foods);

        REQUIRE(visualize_vision(vision) == expected);
    }
};

}  // namespace

TEST_CASE("Eye - Different FOV ranges", "[eye]") {
    using namespace cshorelark::simulation::constants;
    const float pi = k_pi;
    std::vector<food> foods = {create_food(0.5F, 1.0F)};

    SECTION("FOV range 1.0") {
        test_case tc;
        tc.foods = foods;
        tc.fov_angle = pi / 2.0F;
        tc.fov_range = 1.0F;
        tc.x = 0.5F;
        tc.y = 0.5F;
        tc.rot = 0.0F;
        tc.expected = "      +      ";
        tc.run();
    }

    SECTION("FOV range 0.7") {
        test_case tc;
        tc.foods = foods;
        tc.fov_angle = pi / 2.0F;
        tc.fov_range = 0.7F;
        tc.x = 0.5F;
        tc.y = 0.5F;
        tc.rot = 0.0F;
        tc.expected = "      .      ";
        tc.run();
    }

    SECTION("FOV range 0.3") {
        test_case tc;
        tc.foods = foods;
        tc.fov_angle = pi / 2.0F;
        tc.fov_range = 0.3F;
        tc.x = 0.5F;
        tc.y = 0.5F;
        tc.rot = 0.0F;
        tc.expected = "             ";
        tc.run();
    }
}

TEST_CASE("Eye - Different FOV angles", "[eye]") {
    using namespace cshorelark::simulation::constants;
    const float pi = k_pi;
    std::vector<food> foods = {create_food(0.0F, 0.0F),  create_food(0.0F, 0.33F),
                               create_food(0.0F, 0.66F), create_food(0.0F, 1.0F),
                               create_food(1.0F, 0.0F),  create_food(1.0F, 0.33F),
                               create_food(1.0F, 0.66F), create_food(1.0F, 1.0F)};

    SECTION("FOV angle PI/4") {
        test_case tc;
        tc.foods = foods;
        tc.fov_angle = 0.25F * pi;
        tc.fov_range = 1.0F;
        tc.x = 0.5F;
        tc.y = 0.5F;
        tc.rot = 3.0F * pi / 2.0F;
        tc.expected = " +         + ";
        tc.run();
    }

    SECTION("FOV angle PI/2") {
        test_case tc;
        tc.foods = foods;
        tc.fov_angle = 0.5F * pi;
        tc.fov_range = 1.0F;
        tc.x = 0.5F;
        tc.y = 0.5F;
        tc.rot = 3.0F * pi / 2.0F;
        tc.expected = ".  +     +  .";
        tc.run();
    }

    SECTION("FOV angle PI") {
        test_case tc;
        tc.foods = foods;
        tc.fov_angle = pi;
        tc.fov_range = 1.0F;
        tc.x = 0.5F;
        tc.y = 0.5F;
        tc.rot = 3.0F * pi / 2.0F;
        tc.expected = "   . + + .   ";
        tc.run();
    }
}

TEST_CASE("Eye - Different positions", "[eye]") {
    using namespace cshorelark::simulation::constants;
    const float pi = k_pi;
    std::vector<food> foods = {create_food(1.0F, 0.4F), create_food(1.0F, 0.6F)};

    SECTION("Center position") {
        test_case tc;
        tc.foods = foods;
        tc.fov_angle = pi / 2.0F;
        tc.fov_range = 1.0F;
        tc.x = 0.5F;
        tc.y = 0.5F;
        tc.rot = 3.0F * pi / 2.0F;
        tc.expected = "    +   +    ";
        tc.run();
    }

    SECTION("Left edge") {
        test_case tc;
        tc.foods = foods;
        tc.fov_angle = pi / 2.0F;
        tc.fov_range = 1.0F;
        tc.x = 0.1F;
        tc.y = 0.5F;
        tc.rot = 3.0F * pi / 2.0F;
        tc.expected = "     . .     ";
        tc.run();
    }

    SECTION("Right edge") {
        test_case tc;
        tc.foods = foods;
        tc.fov_angle = pi / 2.0F;
        tc.fov_range = 1.0F;
        tc.x = 0.9F;
        tc.y = 0.5F;
        tc.rot = 3.0F * pi / 2.0F;
        tc.expected = "   #     #   ";
        tc.run();
    }
}

TEST_CASE("Eye - Different rotations", "[eye]") {
    using namespace cshorelark::simulation::constants;
    const float pi = k_pi;
    std::vector<food> foods = {create_food(0.0F, 0.5F)};

    SECTION("Looking right (0)") {
        test_case tc;
        tc.foods = foods;
        tc.fov_angle = 2.0F * pi;
        tc.fov_range = 1.0F;
        tc.x = 0.5F;
        tc.y = 0.5F;
        tc.rot = 0.0F;
        tc.expected = "         +   ";
        tc.run();
    }

    SECTION("Looking up (PI/2)") {
        test_case tc;
        tc.foods = foods;
        tc.fov_angle = 2.0F * pi;
        tc.fov_range = 1.0F;
        tc.x = 0.5F;
        tc.y = 0.5F;
        tc.rot = pi / 2.0F;
        tc.expected = "      +      ";
        tc.run();
    }

    SECTION("Looking left (PI)") {
        test_case tc;
        tc.foods = foods;
        tc.fov_angle = 2.0F * pi;
        tc.fov_range = 1.0F;
        tc.x = 0.5F;
        tc.y = 0.5F;
        tc.rot = pi;
        tc.expected = "   +         ";
        tc.run();
    }

    SECTION("Looking down (3PI/2)") {
        test_case tc;
        tc.foods = foods;
        tc.fov_angle = 2.0F * pi;
        tc.fov_range = 1.0F;
        tc.x = 0.5F;
        tc.y = 0.5F;
        tc.rot = 3.0F * pi / 2.0F;
        tc.expected = "      +      ";
        tc.run();
    }
}