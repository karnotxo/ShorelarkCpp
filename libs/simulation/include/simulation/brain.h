#ifndef CSHORELARK_SIMULATION_BRAIN_H
#define CSHORELARK_SIMULATION_BRAIN_H

// C++ system headers
#include <array>
#include <cstddef>
#include <memory>
#include <vector>

// Third-party headers
#include <nonstd/span.hpp>
#include <tl/expected.hpp>

// Project headers
#include "genetic_algorithm/chromosome.h"
#include "neural_network/layer_topology.h"
#include "neural_network/network.h"
#include "random/random.h"
#include "simulation/config.h"
#include "simulation/simulation_error.h"

namespace cshorelark::simulation {

/**
 * @brief Brain class for controlling animal behavior
 *
 * The brain class uses a neural network to process sensory inputs and
 * produce movement outputs for an animal in the simulation.
 */
class brain {
public:
    /**
     * @brief Constructs a brain with specific neural network and config
     * @param config Simulation configuration
     * @param network Pre-configured neural network
     */
    explicit brain(const config& config, neural_network::network<float>&& network);

    /**
     * @brief Brain is non-copyable because it contains a network which is non-copyable
     */
    brain(const brain&) = delete;
    brain& operator=(const brain&) = delete;

    /**
     * @brief Moving is allowed with default implementation
     */
    brain(brain&&) noexcept = default;
    brain& operator=(brain&&) noexcept = default;

    /**
     * @brief Constructs a random brain with specified configuration
     * @param config Brain configuration
     * @param random Random number generator
     */
    [[nodiscard]] static auto random(const config& config,
                                     cshorelark::random::random_generator& random) -> brain;

    /**
     * @brief Processes inputs to produce behavior outputs
     * @param inputs Sensory inputs from the animal's environment
     * @return Expected vector of behavioral outputs (speed, rotation)
     */
    [[nodiscard]] auto propagate(nonstd::span<const float> vision) const
        -> tl::expected<std::vector<float>, simulation_error>;

    /**
     * @brief Gets the neural network weights
     * @return Vector of neural network weights
     */
    [[nodiscard]] auto weights() const -> std::vector<float>;

    /**
     * @brief Gets the number of inputs the brain expects
     * @return Number of input neurons
     */
    [[nodiscard]] auto input_size() const -> std::size_t;

    /**
     * @brief Gets the number of outputs the brain produces
     * @return Number of output neurons
     */
    [[nodiscard]] auto output_size() const -> std::size_t;

    /**
     * @brief Gets the brain as a chromosome
     */
    [[nodiscard]] auto as_chromosome() const -> cshorelark::genetic::chromosome;

    /**
     * @brief Creates a brain from a chromosome
     * @param config Brain configuration
     * @param chromosome Chromosome containing the neural network weights
     * @return Expected brain or error if creation fails
     */
    [[nodiscard]] static auto from_chromosome(const config& config,
                                              const genetic::chromosome& chromosome)
        -> tl::expected<brain, simulation_error>;

    /**
     * @brief Creates a network topology based on configuration
     * @param config Brain configuration
     * @return Array of layer topologies (input, hidden, output)
     */
    [[nodiscard]] static auto topology(const config& config)
        -> std::array<neural_network::layer_topology, 3>;

private:
    float speed_accel_;
    float rotation_accel_;
    neural_network::network<float> network_;

    brain(const config& config, cshorelark::random::random_generator& random);
};

}  // namespace cshorelark::simulation

#endif  // CSHORELARK_SIMULATION_BRAIN_H