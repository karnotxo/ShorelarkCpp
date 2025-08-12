#ifndef SIMULATION_EYE_H
#define SIMULATION_EYE_H

// Related headers
#include "simulation/config.h"
#include "simulation/food.h"
#include "simulation/vector2d.h"

// C++ system headers
#include <cstddef>
#include <vector>

// Third-party headers
#include <nonstd/span.hpp>

namespace cshorelark::simulation {

/**
 * @brief Simulates vision with a set of photoreceptors arranged in an arc.
 *
 * @details The eye class provides a simple model of vision using a set of photoreceptors
 * arranged in an arc. Each receptor detects the presence of food within its
 * field of view and range.
 *
 * The receptors are evenly spaced across the field of view, with the central
 * receptor aligned with the animal's forward direction. *
 */
class eye {
public:
    /**
     * @brief Constructs an eye with specified field of view.
     * @param fov_deg Field of view in degrees
     * @throws std::invalid_argument if fov_deg is not in (0, 360)
     */
    explicit eye(float fov_range, float fov_deg, size_t num_cells);

    /**
     * @brief Constructs an eye with given configuration.
     * @param cfg Configuration parameters
     * @throws std::invalid_argument if config contains invalid values
     */
    explicit eye(const brain_eye_config& cfg = brain_eye_config{});

    /**
     * @brief Updates the eye's view of the world.
     *
     * @details Casts rays from the current position in the direction of each receptor
     * to detect food in the world. The intensity of each receptor's response
     * is inversely proportional to the distance to the nearest food item.
     *
     * @param position Current position in the world
     * @param rotation Current rotation in radians
     * @param food_items Collection of food items to detect (as a span)
     * @return Visual input from the environment
     * @throws std::invalid_argument if position or rotation is invalid
     */
    [[nodiscard]] auto process_vision(const vector2d& position, float rotation,
                                      nonstd::span<const food> food_items) const
        -> std::vector<float>;

    /**
     * @brief Gets the number of photoreceptors.
     * @return Number of receptors
     */
    [[nodiscard]] auto get_num_receptors() const noexcept -> int {
        return static_cast<int>(cells_);
    }

    /**
     * @brief Gets the field of view in degrees.
     * @return Field of view angle
     */
    [[nodiscard]] auto get_fov_degrees() const noexcept -> float;

private:
    float fov_range_;  ///< Field of view range
    float fov_angle_;  ///< Field of view angle in radians
    size_t cells_;     ///< Number of photoreceptors
};

}  // namespace cshorelark::simulation

#endif  // SIMULATION_EYE_H