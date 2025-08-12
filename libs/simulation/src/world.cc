#include "simulation/world.h"

// Include complete definitions for animal, eye, and brain
#include <spdlog/spdlog.h>

#include "random/random.h"
#include "simulation/animal.h"
#include "simulation/config.h"
#include "simulation/food.h"

// C++ system headers
#include <cstddef>  // for std::size_t
#include <utility>
#include <vector>  // for std::vector

namespace cshorelark::simulation {

// Define world's destructor here where animal is complete
world::~world() = default;

auto world::random(const config& cfg, cshorelark::random::random_generator& random) -> world {
    auto foods = std::vector<food>{};
    foods.reserve(cfg.world.num_foods);
    auto animals = std::vector<animal>{};
    animals.reserve(cfg.world.num_animals);

    // Create random animals
    for (size_t i = 0; i < cfg.world.num_animals; i++) {
        auto new_animal = animal::random(cfg, random);
        animals.push_back(std::move(new_animal));
    }

    // Create random foods
    for (size_t i = 0; i < cfg.world.num_foods; i++) {
        auto new_food = food::random(random);
        foods.push_back(new_food);
    }

    return world(std::move(animals), std::move(foods));
}

}  // namespace cshorelark::simulation