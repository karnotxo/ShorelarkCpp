#ifndef CSHORELARK_SIMULATION_ANIMAL_H
#define CSHORELARK_SIMULATION_ANIMAL_H

// C++ system headers
#include <cstddef>  // For std::size_t
#include <vector>

// External library headers
#include <nonstd/span.hpp>

// Project headers
#include "genetic_algorithm/chromosome.h"
#include "random/random.h"
#include "simulation/brain.h"
#include "simulation/config.h"
#include "simulation/eye.h"
#include "simulation/food.h"
#include "simulation/simulation_error.h"
#include "simulation/vector2d.h"
#include "tl/expected.hpp"

namespace cshorelark::simulation {

/**
 * @brief Represents an animal in the simulation
 */
class animal {
public:
    /**
     * @brief Gets the current position of the animal
     * @return Animal position
     */
    [[nodiscard]] auto position() const noexcept -> const vector2d& { return position_; }

    /**
     * @brief Sets the position of the animal
     * @param position New position
     */
    void set_position(const vector2d& position) noexcept;

    /**
     * @brief Gets the current rotation/heading of the animal
     * @return Rotation in radians
     */
    [[nodiscard]] auto rotation() const noexcept -> float { return rotation_; }

    /**
     * @brief Sets the rotation/heading of the animal
     * @param rotation New rotation in radians
     */
    void set_rotation(float rotation) noexcept { rotation_ = rotation; }

    /**
     * @brief Gets the current speed of the animal
     * @return Speed in world units
     */
    [[nodiscard]] auto speed() const noexcept -> float { return speed_; }

    /**
     * @brief Sets the speed of the animal
     * @param speed New speed in world units
     */
    void set_speed(float speed) noexcept { speed_ = speed; }

    /**
     * @brief Gets the current vision of the animal
     * @return Vector of vision data
     */
    [[nodiscard]] auto vision() const noexcept -> const std::vector<float>& { return vision_; }

    /**
     * @brief Get the amount of food eaten
     * @return Food eaten count
     */
    [[nodiscard]] auto food_eaten() const noexcept -> std::size_t { return food_eaten_; }

    /**
     * @brief Increments the food eaten counter
     */
    void increment_food_eaten() noexcept { food_eaten_++; }

    /**
     * @brief Creates a random animal
     * @param config Configuration settings
     * @param random Random generator
     * @return Newly created animal
     */
    [[nodiscard]] static auto random(const config& config,
                                     cshorelark::random::random_generator& random) -> animal;

    /**
     * @brief Creates an animal from a chromosome
     *
     * @param config Configuration for the animal
     * @param random Random generator for initialization
     * @param chromosome Chromosome containing the neural network weights
     * @return A new animal instance
     */
    static auto from_chromosome(const config& config, random::random_generator& random,
                                const genetic::chromosome& chromosome)
        -> tl::expected<animal, simulation_error>;

    /**
     * @brief Converts the animal's brain weights to a chromosome
     *
     * @return A chromosome representing this animal's brain
     */
    [[nodiscard]] auto as_chromosome() const -> genetic::chromosome;

    /**
     * @brief Process brain outputs to determine movement
     * @param config Configuration settings
     * @param foods Collection of food items in the world
     */
    void process_brain(const config& config, nonstd::span<const food> foods);

    /**
     * @brief Process the animal's movement based on speed and rotation
     */
    void process_movement();

private:
    vector2d position_;          ///< Current position
    float rotation_;             ///< Current rotation/heading (in radians)
    std::vector<float> vision_;  ///< Current vision
    float speed_;                ///< Current speed
    eye eye_;                    ///< The animal's eye
    brain brain_;                ///< Neural network brain
    std::size_t food_eaten_{0};  ///< Food eaten counter

    animal(const config& config, cshorelark::random::random_generator& random, brain brain);

    void set_position(const float& pos_x, const float& pos_y) noexcept;
};

}  // namespace cshorelark::simulation

#endif  // CSHORELARK_SIMULATION_ANIMAL_H