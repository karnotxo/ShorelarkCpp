#include "simulation/food.h"  // Related header

#include "random/random.h"
#include "simulation/vector2d.h"

namespace cshorelark::simulation {

food::food(const vector2d& position) : position_(position) {}

auto food::position() const noexcept -> vector2d { return position_; }

void food::set_position(const vector2d& position) noexcept { position_ = position; }

void food::randomize_position(cshorelark::random::random_generator& random) noexcept {
    position_.set_x(random.generate_position());
    position_.set_y(random.generate_position());
}

auto food::random(cshorelark::random::random_generator& random) -> food {
    return food{vector2d{random.generate_position(), random.generate_position()}};
}
}  // namespace cshorelark::simulation