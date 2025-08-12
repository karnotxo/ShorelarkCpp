#include "simulation/eye.h"

#include <cmath>
#include <stdexcept>

#include "simulation/config.h"

namespace cshorelark::simulation {
// Constants
namespace {
constexpr float k_degree_to_radian = k_pi / 180.0F;
}  // namespace
eye::eye(float fov_range, float fov_deg, size_t num_cells)
    : fov_range_(fov_range), fov_angle_(fov_deg * k_degree_to_radian), cells_(num_cells) {}

eye::eye(const brain_eye_config& cfg)
    : fov_range_(cfg.fov_range),
      fov_angle_(cfg.fov_angle_deg * k_degree_to_radian),
      cells_(cfg.num_cells) {}

auto eye::process_vision(const vector2d& position, float rotation,
                         nonstd::span<const food> food_items) const -> std::vector<float> {
    // Create new vision with the correct number of receptors
    std::vector<float> cells(cells_);

    for (const auto& food_value : food_items) {
        // Calculate vector from position to food
        const vector2d to_food = food_value.position() - position;

        // Calculate distance to food
        const float distance = to_food.length();

        // Skip if food is too far away (optimization)
        if (distance > fov_range_) {
            continue;
        }

        // In Rust: let angle = na::Rotation2::rotation_between(&na::Vector2::y(),
        // &vec).angle(); Calculate the angle between the y-axis (0, 1) and the to_food vector
        // We use atan2 to get the angle in the correct quadrant
        const float to_food_angle =
            std::atan2f(to_food.x(), to_food.y());  // Notice y first, then x for angle from y-axis

        // Calculate angle difference between ray direction and vector to food
        // Normalize the angle difference to be within [-π, π]
        float angle_diff = to_food_angle - rotation;
        while (angle_diff > constants::k_pi)
            angle_diff -= constants::k_two_pi;
        while (angle_diff < -constants::k_pi)
            angle_diff += constants::k_two_pi;

        // If the angle difference is too large, the food is outside our field of view
        const float fov_half_rad = fov_angle_ / 2;
        if (std::abs(angle_diff) > fov_half_rad) {
            continue;
        }
        angle_diff = angle_diff + fov_half_rad;  // Changed 'let' to 'float' for C++ syntax
        int cell = static_cast<int>(angle_diff / fov_angle_ * static_cast<float>(cells_));
        cell = std::min(cell, static_cast<int>(cells_ - 1));

        cells[cell] += (fov_range_ - distance) / fov_range_;
    }
    return cells;
}

auto eye::get_fov_degrees() const noexcept -> float { return fov_angle_ * k_degree_to_radian; }
}  // namespace cshorelark::simulation
