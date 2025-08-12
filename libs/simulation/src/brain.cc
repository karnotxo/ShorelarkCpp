#include "simulation/brain.h"

#include "genetic_algorithm/chromosome.h"
#include "neural_network/layer_topology.h"
#include "neural_network/network.h"
#include "nonstd/span.hpp"
#include "random/random.h"
#include "simulation/constants.h"
#include "tl/expected.hpp"

namespace cshorelark::simulation {

brain::brain(const config& config, neural_network::network<float>&& network)
    : speed_accel_(config.sim.speed_accel),
      rotation_accel_(config.sim.rotation_accel_deg * constants::k_deg_to_rad),
      network_(std::move(network)) {}

brain::brain(const config& config, cshorelark::random::random_generator& random)
    : speed_accel_(config.sim.speed_accel),
      rotation_accel_(config.sim.rotation_accel_deg * constants::k_deg_to_rad),
      network_(std::move(*neural_network::network<float>::random(topology(config), random))) {}

auto brain::propagate(nonstd::span<const float> vision) const
    -> tl::expected<std::vector<float>, simulation_error> {
    // Get raw neural network response
    auto result = network_.propagate(vision);

    // Check if propagation was successful
    if (!result) {
        // Map the network error to a corresponding simulation error
        switch (result.error()) {
            case neural_network::network_error::k_invalid_input_size:
                return tl::make_unexpected(simulation_error::k_invalid_input_size);
            case neural_network::network_error::k_propagation_error:
                return tl::make_unexpected(simulation_error::k_layer_propagation_failed);
            case neural_network::network_error::k_invalid_layer_count:
                return tl::make_unexpected(simulation_error::k_invalid_network_structure);
            default:
                return tl::make_unexpected(simulation_error::k_neural_network_error);
        }
    }

    const auto& response = result.value();

    // Handle empty response
    if (response.size() < 2) {
        return tl::make_unexpected(simulation_error::k_insufficient_outputs);
    }

    // Following the Rust implementation logic:
    // let r0 = response[0].clamp(0.0, 1.0) - 0.5;
    // let r1 = response[1].clamp(0.0, 1.0) - 0.5;
    // let speed = (r0 + r1).clamp(-self.speed_accel, self.speed_accel);
    // let rotation = (r0 - r1).clamp(-self.rotation_accel, self.rotation_accel);

    // Clamp and normalize the outputs
    float r0 = std::clamp(response[0], 0.0F, 1.0F) - 0.5F;
    float r1 = std::clamp(response[1], 0.0F, 1.0F) - 0.5F;

    // Calculate speed and rotation from normalized outputs
    const float speed = std::clamp(r0 + r1, -speed_accel_, speed_accel_);
    const float rotation = std::clamp(r0 - r1, -rotation_accel_, rotation_accel_);

    return std::vector<float>{speed, rotation};
}

auto brain::weights() const -> std::vector<float> { return network_.weights(); }

auto brain::input_size() const -> std::size_t { return network_.input_size(); }

auto brain::output_size() const -> std::size_t { return network_.output_size(); }

auto brain::topology(const config& config) -> std::array<neural_network::layer_topology, 3> {
    return std::array<neural_network::layer_topology, 3>{
        neural_network::layer_topology{config.brain_eye.num_cells},
        neural_network::layer_topology{config.brain_eye.num_neurons},
        neural_network::layer_topology{2}  // Always 2 output neurons (speed and rotation)
    };
}

auto brain::as_chromosome() const -> cshorelark::genetic::chromosome {
    return cshorelark::genetic::chromosome(network_.weights());
}

auto brain::from_chromosome(const config& config, const genetic::chromosome& chromosome)
    -> tl::expected<brain, simulation_error> {
    // Create a local copy of the topologies to ensure proper lifetime
    // Instead of using a span that points to thread-local storage
    const std::array<neural_network::layer_topology, 3> topologies = topology(config);

    // Create a span from our local copy
    const auto topologies_span = nonstd::span<const neural_network::layer_topology>(topologies);

    // Convert chromosome to vector of weights
    const auto& weights = chromosome.genes();

    // Create neural network from weights
    auto network_result = neural_network::network<float>::from_weights(topologies_span, weights);

    if (!network_result) {
        // Map network error to simulation error
        auto error = network_result.error();
        switch (error) {
            case neural_network::network_error::k_invalid_layer_count:
                return tl::unexpected(simulation_error::k_invalid_brain_config);
            case neural_network::network_error::k_not_enough_weights:
            case neural_network::network_error::k_too_many_weights:
                return tl::unexpected(simulation_error::k_invalid_chromosome);
            case neural_network::network_error::k_invalid_layer_topology:
                return tl::unexpected(simulation_error::k_invalid_brain_config);
            case neural_network::network_error::k_network_not_initialized:
            case neural_network::network_error::k_invalid_input_size:
            case neural_network::network_error::k_propagation_error:
                return tl::unexpected(simulation_error::k_brain_operation_failed);
        }
    }

    // Successfully created the network from weights
    return brain(config, std::move(network_result.value()));
}

auto brain::random(const config& config, cshorelark::random::random_generator& random) -> brain {
    // Create a local copy of the topologies to ensure proper lifetime
    const std::array<neural_network::layer_topology, 3> topologies = topology(config);

    // Create a span from our local copy
    const auto topologies_span = nonstd::span<const neural_network::layer_topology>(topologies);

    return brain(config,
                 std::move(*neural_network::network<float>::random(topologies_span, random)));
}

}  // namespace cshorelark::simulation