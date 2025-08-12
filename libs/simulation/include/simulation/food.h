#ifndef CSHORELARK_SIMULATION_FOOD_H
#define CSHORELARK_SIMULATION_FOOD_H

#include "random/random.h"
#include "simulation/vector2d.h"

namespace cshorelark::simulation {

/**
 * @brief Represents a food item in the world.
 */
struct alignas(8) food {
private:
    vector2d position_;  ///< Position in the world

public:
    explicit food(const vector2d& position);

    [[nodiscard]] auto position() const noexcept -> vector2d;

    void set_position(const vector2d& position) noexcept;

    void randomize_position(cshorelark::random::random_generator& random) noexcept;

    [[nodiscard]] static auto random(cshorelark::random::random_generator& random) -> food;
};

}  // namespace cshorelark::simulation

#endif  // CSHORELARK_SIMULATION_FOOD_H