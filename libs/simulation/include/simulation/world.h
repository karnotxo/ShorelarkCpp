#ifndef CSHORELARK_SIMULATION_WORLD_H
#define CSHORELARK_SIMULATION_WORLD_H

// Related headers
#include "genetic_algorithm/genetic_algorithm.h"
#include "neural_network/network.h"
#include "random/random.h"
#include "simulation/config.h"
#include "simulation/constants.h"
#include "simulation/food.h"
#include "simulation/vector2d.h"

// C++ system headers
#include <cstddef>
#include <memory>
#include <random>
#include <string>
#include <utility>
#include <vector>

namespace cshorelark::simulation {

// Forward declarations
class animal;

/**
 * @brief Simulation world containing animals and food.
 */
class world {
public:
    /**
     * @brief Constructs a new world with given animals and foods.
     * @param animals Vector of animals to store
     * @param foods Vector of foods to store
     */
    explicit world(std::vector<animal>&& animals, std::vector<food>&& foods)
        : animals_(std::move(animals)), foods_(std::move(foods)) {}

    // Delete copy constructor and copy assignment operator to prevent copying
    world(const world&) = delete;
    world& operator=(const world&) = delete;

    /**
     * @brief Moving is allowed and defaulted
     */
    world(world&&) noexcept = default;

    /**
     * @brief Moving assignment is allowed and defaulted
     */
    world& operator=(world&&) noexcept = default;

    /**
     * @brief Gets all food items in the world.
     * @return Vector of food items
     */
    [[nodiscard]] auto get_foods() const -> std::vector<food>& { return foods_; }

    [[nodiscard]] auto foods_count() const -> std::size_t { return foods_.size(); }

    /**
     * @brief Gets all animals in the world.
     * @return Vector of animals
     */
    [[nodiscard]] auto get_animals() const -> std::vector<animal>& { return animals_; }

    /**
     * @brief Sets the animals in the world.
     * @param animals Vector of animals to set
     */
    auto set_animals(std::vector<animal>&& animals) -> void { animals_ = std::move(animals); }

    [[nodiscard]] static auto random(const config& cfg,
                                     cshorelark::random::random_generator& random) -> world;

    // Destructor declaration (defined in world.cc where animal is complete type)
    ~world();

private:
    ///< Configuration parameters
    mutable std::vector<animal> animals_;  ///< Animals in the world
    mutable std::vector<food> foods_;      ///< Food items in the world
};

}  // namespace cshorelark::simulation

#endif  // CSHORELARK_SIMULATION_WORLD_H