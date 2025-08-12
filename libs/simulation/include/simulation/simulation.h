#ifndef CSHORELARK_SIMULATION_SIMULATION_H
#define CSHORELARK_SIMULATION_SIMULATION_H

#include <cstddef>
#include <optional>

#include "random/random.h"
#include "simulation/config.h"
#include "simulation/statistics.h"
#include "simulation/world.h"

namespace cshorelark::simulation {

/**
 * @brief Manages the complete simulation lifecycle
 *
 * The Simulation class orchestrates the entire simulation process, including
 * animal behavior, world interactions, and evolution across generations.
 */
class simulation {
public:
    /**
     * @brief Creates a new simulation with random components
     *
     * @param config Configuration settings for the simulation
     * @param random Random number generator
     * @return A new simulation instance
     */
    static auto random(const config& config,
                       cshorelark::random::random_generator& random) -> simulation;

    /**
     * @brief Creates a new food in random position
     *
     * @param random Random number generator
     *
     * @return void
     */
    void spawn_food(cshorelark::random::random_generator& random);

    /**
     * @brief Creates a new food at a specific position
     *
     * @param pos_x X-coordinate of the food position (normalized)
     * @param pos_y Y-coordinate of the food position (normalized)
     */
    void spawn_food(float pos_x, float pos_y);

    /**
     * @brief Creates a new animal in random position
     *
     * @param random Random number generator
     *
     * @return void
     */
    void spawn_animal(cshorelark::random::random_generator& random);

    /**
     * @brief Get the current configuration
     *
     * @return Reference to the simulation configuration
     */
    [[nodiscard]] auto get_config() const -> const config& { return config_; }

    /**
     * @brief Get the current simulation world
     *
     * @return Reference to the world
     */
    [[nodiscard]] auto get_world() const -> const world& { return world_; }

    /**
     * @brief Get the current generation
     *
     * @return Reference to the current generation
     */
    [[nodiscard]] auto get_generation() const -> std::size_t { return generation_; }

    /**
     * @brief Get the current age
     *
     * @return Current age of the simulation
     */
    [[nodiscard]] auto get_age() const -> std::size_t { return age_; }

    /**
     * @brief Advance the simulation by one step
     *
     * @param random Random number generator
     * @return Optional statistics if a generation completed, or nullopt
     */
    auto step(cshorelark::random::random_generator& random)
        -> std::optional<cshorelark::simulation::statistics>;

    /**
     * @brief Runs a complete training cycle until the next generation
     *
     * @param random Random number generator
     * @return Statistics for the completed training
     */
    auto train(cshorelark::random::random_generator& random) -> cshorelark::simulation::statistics;

private:
    /**
     * @brief Constructor
     *
     * @param config Simulation configuration
     * @param world Initial world state
     */
    simulation(config config, world&& world);

    /**
     * @brief Processes collisions between animals and food
     *
     * @param random Random generator for food repositioning
     */
    void process_collisions(cshorelark::random::random_generator& random);

    /**
     * @brief Process brain calculations for all animals
     */
    void process_brains();

    /**
     * @brief Process movement for all animals
     */
    void process_movements();

    /**
     * @brief Check if it's time to evolve and perform evolution
     *
     * @param random Random generator for evolution
     * @return Optional statistics if evolution occurred, or nullopt
     */
    auto try_evolving(cshorelark::random::random_generator& random)
        -> std::optional<cshorelark::simulation::statistics>;

    /**
     * @brief Perform genetic evolution of the population
     *
     * @param random Random generator
     * @return Statistics for this evolution
     */
    auto evolve(cshorelark::random::random_generator& random) -> cshorelark::simulation::statistics;

    config config_;               ///< Simulation configuration
    world world_;                 ///< Current world state
    std::size_t age_ = 0;         ///< Current age (steps since last evolution)
    std::size_t generation_ = 0;  ///< Current generation counter
};

}  // namespace cshorelark::simulation

#endif  // CSHORELARK_SIMULATION_SIMULATION_H