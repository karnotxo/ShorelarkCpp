#include "simulation/animal.h"

// Include required definitions for eye and brain classes
#include <spdlog/spdlog.h>  // Add spdlog include for logging

#include <algorithm>
#include <cmath>
#include <nonstd/span.hpp>  // Add span include

#include "genetic_algorithm/chromosome.h"
#include "simulation/config.h"
#include "simulation/constants.h"
#include "simulation/eye.h"
#include "simulation/food.h"
#include "simulation/vector2d.h"
#include "simulation/world.h"
#include "tl/expected.hpp"

namespace cshorelark::simulation {

// Utility function to wrap a value within a range (min, max)
// Equivalent to Rust's na::wrap function
static float wrap(float value, float min, float max) {
    const float width = max - min;
    // First, bring the value into the range [min, min+width)
    float wrapped_value = std::fmod(value - min, width);
    // Handle negative values (fmod can return negative)
    if (wrapped_value < 0.0f) {
        wrapped_value += width;
    }
    return wrapped_value + min;
}

animal::animal(const config& config, cshorelark::random::random_generator& random, brain brain)
    : position_(random.generate_position(), random.generate_position()),
      rotation_(random.generate_rotation()),
      speed_(config.sim.speed_max),
      eye_(eye(config.brain_eye)),
      brain_(std::move(brain)) {
    spdlog::debug("Animal created at position=({},{}), rotation={}", position_.x(), position_.y(),
                  rotation_);
}

void animal::process_brain(const config& config, nonstd::span<const food> foods) {
    vision_ = eye_.process_vision(position_, rotation_, foods);

    // Process inputs through the neural network
    auto outputs_result = brain_.propagate(vision_);

    if (!outputs_result) {
        spdlog::error("Failed to process brain inputs: error code {}",
                      static_cast<int>(outputs_result.error()));
        return;
    }

    // Process the outputs for movement
    speed_ += outputs_result.value().at(0);
    speed_ = std::clamp(speed_, config.sim.speed_min, config.sim.speed_max);
    rotation_ += outputs_result.value().at(1);
    rotation_ = std::fmod(rotation_, k_two_pi);  // Normalize rotation to [0, 2π)

    spdlog::debug("After processing: speed={:.4f}, rotation={:.4f}", speed_, rotation_);
}

void animal::process_movement() {
    spdlog::debug("update_position: speed={}, rotation={}", speed_, rotation_);

    // Calculate movement vector based on current rotation and speed
    const float new_x = position_.x() + speed_ * std::cosf(rotation_);
    const float new_y = position_.y() + speed_ * std::sinf(rotation_);

    // Update position - wrap position to keep within bounds [0,1]
    position_.set_x(wrap(new_x, 0.0F, 1.0F));
    position_.set_y(wrap(new_y, 0.0F, 1.0F));
}

void animal::set_position(const vector2d& position) noexcept {
    // Update position - wrap position to keep within bounds [0,1]
    set_position(position.x(), position.y());
}

void animal::set_position(const float& pos_x, const float& pos_y) noexcept {
    // Update position - wrap position to keep within bounds [0,1]
    position_.set_x(wrap(pos_x, 0.0F, 1.0F));
    position_.set_y(wrap(pos_y, 0.0F, 1.0F));
}

auto animal::random(const config& config, cshorelark::random::random_generator& random) -> animal {
    auto brain = brain::random(config, random);
    return {config, random, std::move(brain)};
}

auto animal::from_chromosome(const config& config, random::random_generator& random,
                             const genetic::chromosome& chromosome)
    -> tl::expected<animal, simulation_error> {
    // Create brain from chromosome using move semantics
    auto brain_result = brain::from_chromosome(config, chromosome);

    if (!brain_result) {
        // If brain creation fails, create a random brain as fallback
        spdlog::warn("Failed to create brain from chromosome: {}",
                     static_cast<int>(brain_result.error()));
        return tl::unexpected<simulation_error>{simulation_error::k_invalid_chromosome};
    }

    return {animal{config, random, std::move(brain_result.value())}};
}

auto animal::as_chromosome() const -> genetic::chromosome {
    // Return the brain's weights as a chromosome
    return genetic::chromosome(brain_.weights());
}

}  // namespace cshorelark::simulation