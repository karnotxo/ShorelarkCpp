#include "neural_network/network.h"

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <cstddef>
#include <limits>
#include <vector>

#include "neural_network/layer.h"
#include "neural_network/layer_topology.h"
#include "neural_network/neuron.h"
#include "random/random.h"

using Catch::Matchers::WithinRel;
using cshorelark::neural_network::layer;
using cshorelark::neural_network::layer_topology;
using cshorelark::neural_network::network;
using cshorelark::neural_network::network_error;
using cshorelark::neural_network::neuron;
using cshorelark::random::random_generator;

namespace {
// Constants for test configuration
constexpr float k_weight_1 = 0.1F;
constexpr float k_weight_2 = 0.2F;
constexpr float k_weight_3 = 0.3F;
constexpr float k_weight_4 = 0.4F;
constexpr float k_weight_5 = 0.5F;
constexpr float k_weight_6 = 0.6F;
constexpr float k_weight_7 = 0.7F;
constexpr float k_weight_8 = 0.8F;
constexpr float k_weight_9 = 0.9F;
constexpr float k_bias_zero = 0.0F;
constexpr float k_bias_1 = 0.5F;
constexpr float k_bias_2 = 0.6F;
constexpr float k_bias_3 = 0.7F;
constexpr float k_bias_4 = 0.1F;
constexpr float k_input_1 = 1.0F;
constexpr float k_input_2 = 2.0F;
constexpr float k_input_3 = 3.0F;
constexpr float k_negative_weight = -0.5F;
constexpr float k_expected_output = 0.5F;
constexpr std::size_t k_layer_size_1 = 1;
constexpr std::size_t k_layer_size_2 = 2;
constexpr std::size_t k_layer_size_3 = 3;
constexpr std::size_t k_expected_weights_13 = 13;
constexpr int k_random_seed = 42;
constexpr float k_weight_range_min = -1.0F;
constexpr float k_weight_range_max = 1.0F;
}  // namespace

TEST_CASE("Network - Basic Construction", "[network]") {
    SECTION("Construction with layers") {
        std::vector<layer<float>> layers;

        // First layer: 2 inputs -> 3 neurons
        std::vector<neuron<float>> layer1_neurons;
        layer1_neurons.emplace_back(k_bias_1, std::vector<float>{k_weight_1, k_weight_2});
        layer1_neurons.emplace_back(k_bias_2, std::vector<float>{k_weight_3, k_weight_4});
        layer1_neurons.emplace_back(k_bias_3, std::vector<float>{k_weight_5, k_weight_6});
        auto layer1 = layer<float>::create(std::move(layer1_neurons));
        REQUIRE(layer1.has_value());
        layers.push_back(std::move(*layer1));

        // Second layer: 3 inputs -> 1 neuron
        std::vector<neuron<float>> layer2_neurons;
        layer2_neurons.emplace_back(k_bias_4,
                                    std::vector<float>{k_weight_7, k_weight_8, k_weight_9});
        auto layer2 = layer<float>::create(std::move(layer2_neurons));
        REQUIRE(layer2.has_value());
        layers.push_back(std::move(*layer2));

        network<float> network1(std::move(layers));
        CHECK(network1.input_size() == k_layer_size_2);
        CHECK(network1.output_size() == k_layer_size_1);
    // Expected: (inputs + bias) * neurons for each layer: (2+1)*3 + (3+1)*1 = 9 + 4 = 13
    CHECK(network1.weight_count() == ((k_layer_size_2 + 1) * k_layer_size_3 +
                      (k_layer_size_3 + 1) * k_layer_size_1));
    }
}

TEST_CASE("Network - FromWeights Construction - Valid weights", "[network]") {
    std::vector<layer_topology> topology = {
        layer_topology(k_layer_size_2),  // Input layer: 2 neurons
        layer_topology(k_layer_size_1)   // Output layer: 1 neuron (simplified)
    };

    // Weights for single layer: 1 neuron with 2 inputs + bias = 3 weights
    std::vector<float> weights = {k_bias_1, k_weight_1, k_weight_2};

    auto network1 = network<float>::from_weights(topology, weights);
    REQUIRE(network1.has_value());
    CHECK(network1->input_size() == k_layer_size_2);
    CHECK(network1->output_size() == k_layer_size_1);
    CHECK(network1->weight_count() == 3);
}

TEST_CASE("Network - FromWeights Construction - Not enough weights", "[network]") {
    std::printf("DEBUG: Starting not enough weights test\n");
    std::vector<layer_topology> topology = {layer_topology(k_layer_size_2),
                                            layer_topology(k_layer_size_3),
                                            layer_topology(k_layer_size_1)};

    std::vector<float> weights = {k_bias_1, k_weight_1, k_weight_2};  // Too few weights
    std::printf("DEBUG: About to call from_weights with insufficient weights\n");
    auto network1 = network<float>::from_weights(topology, weights);
    std::printf("DEBUG: from_weights returned\n");
    REQUIRE(!network1.has_value());
    std::printf("DEBUG: About to check error type\n");
    CHECK(network1.error() == network_error::k_not_enough_weights);
    std::printf("DEBUG: Test completed\n");
}

TEST_CASE("Network - FromWeights Construction - Too many weights", "[network]") {
    std::vector<layer_topology> topology = {layer_topology(k_layer_size_2),
                                            layer_topology(k_layer_size_1)};

    std::vector<float> weights = {k_bias_1, k_weight_1, k_weight_2, k_weight_3,
                                  k_weight_4};  // Extra weights
    auto network1 = network<float>::from_weights(topology, weights);
    REQUIRE(!network1.has_value());
    CHECK(network1.error() == network_error::k_too_many_weights);
}

TEST_CASE("Network - FromWeights Construction - Invalid layer count", "[network]") {
    std::vector<layer_topology> topology = {layer_topology(k_layer_size_2)};  // Only one layer
    std::vector<float> weights = {k_weight_1, k_weight_2};
    auto network1 = network<float>::from_weights(topology, weights);
    REQUIRE(!network1.has_value());
    CHECK(network1.error() == network_error::k_invalid_layer_count);
}

TEST_CASE("Network - Random Creation", "[network]") {
    random_generator random_gen(k_random_seed);  // Fixed seed for reproducibility

    SECTION("Creates correct structure") {
        std::vector<layer_topology> topology = {layer_topology(k_layer_size_2),
                                                layer_topology(k_layer_size_3),
                                                layer_topology(k_layer_size_1)};

        auto network1 = network<float>::random(topology, random_gen);
        REQUIRE(network1.has_value());
        CHECK(network1->input_size() == k_layer_size_2);
        CHECK(network1->output_size() == k_layer_size_1);
        CHECK(network1->weight_count() == k_expected_weights_13);

        // Check all weights are within [-1, 1]
        auto weights = network1->weights();
        for (const float weight : weights) {
            CHECK(weight >= k_weight_range_min);
            CHECK(weight <= k_weight_range_max);
        }
    }

    SECTION("Invalid layer count") {
        const std::vector<layer_topology> topology = {layer_topology(k_layer_size_2)};
        auto network1 = network<float>::random(topology, random_gen);
        REQUIRE(!network1.has_value());
        CHECK(network1.error() == network_error::k_invalid_layer_count);
    }
}

TEST_CASE("Network - Process", "[network]") {
    SECTION("Valid input propagation") {
        // Create a simple network: 2 -> 1
        std::vector<layer<float>> layers;
        std::vector<neuron<float>> neurons;
        neurons.emplace_back(
            k_bias_zero,
            std::vector<float>{k_weight_5, k_negative_weight});  // weights: 0.5, -0.5, bias: 0
        auto layer_result = layer<float>::create(std::move(neurons));
        REQUIRE(layer_result.has_value());
        layers.push_back(std::move(*layer_result));

        network<float> network1(std::move(layers));

        std::vector<float> inputs = {k_input_1, k_input_1};
        auto result = network1.propagate(inputs);
        REQUIRE(result.has_value());
        REQUIRE(result->size() == 1);

        // Manual calculation using ReLU: max(0, 1.0*0.5 + 1.0*-0.5 + 0.0) = max(0, 0.0) = 0.0
        const float expected =
            std::max(0.0F, k_input_1 * k_weight_5 + k_input_1 * k_negative_weight + k_bias_zero);
        CHECK_THAT((*result)[0], WithinRel(expected));
    }

    SECTION("Invalid input size") {
        std::vector<layer<float>> layers;
        std::vector<neuron<float>> neurons;
        neurons.emplace_back(k_bias_zero, std::vector<float>{k_weight_5});
        auto layer1 = layer<float>::create(std::move(neurons));
        REQUIRE(layer1.has_value());
        layers.push_back(std::move(*layer1));

        network<float> network1(std::move(layers));

        std::vector<float> inputs = {k_input_1, k_input_1};  // Too many inputs
        auto result = network1.propagate(inputs);
        REQUIRE(!result.has_value());
        CHECK(result.error() == network_error::k_invalid_input_size);
    }
}

TEST_CASE("Network - Weights round-trip", "[network]") {
    // Build a simple topology 2 -> 3 -> 1
    std::vector<layer_topology> topology = {layer_topology(k_layer_size_2),
                                            layer_topology(k_layer_size_3),
                                            layer_topology(k_layer_size_1)};

    // Provide explicit weights bias-first per neuron
    std::vector<float> weights = {k_bias_1, k_weight_1, k_weight_2,    // neuron 1
                                  k_bias_2, k_weight_3, k_weight_4,    // neuron 2
                                  k_bias_3, k_weight_5, k_weight_6,    // neuron 3
                                  k_bias_4, k_weight_7, k_weight_8, k_weight_9};  // output neuron

    auto net_result = network<float>::from_weights(topology, weights);
    REQUIRE(net_result.has_value());
    auto flattened = net_result->weights();
    REQUIRE(flattened.size() == weights.size());
    for (std::size_t i = 0; i < weights.size(); ++i) {
        CHECK_THAT(flattened[i], WithinRel(weights[i]));
    }

    // Reconstruct from the flattened weights again and compare
    auto net_result2 = network<float>::from_weights(topology, flattened);
    REQUIRE(net_result2.has_value());
    auto flattened2 = net_result2->weights();
    REQUIRE(flattened2.size() == weights.size());
    for (std::size_t i = 0; i < weights.size(); ++i) {
        CHECK_THAT(flattened2[i], WithinRel(weights[i]));
    }
}

TEST_CASE("Network - Error Handling", "[neural-network]") {
    SECTION("Invalid layer count") {
        std::vector<layer_topology> topology = {layer_topology(k_layer_size_3)};
        random_generator random_gen(k_random_seed);

        auto result = network<float>::random(topology, random_gen);
        REQUIRE(!result.has_value());
        REQUIRE(result.error() == network_error::k_invalid_layer_count);
    }

    SECTION("Invalid input size") {
        std::vector<layer_topology> topology = {layer_topology(k_layer_size_3),
                                                layer_topology(k_layer_size_2)};
        random_generator random_gen(k_random_seed);

        auto network1 = network<float>::random(topology, random_gen);
        REQUIRE(network1.has_value());

        std::vector<float> wrong_size_input = {k_input_1, k_input_2};  // Should be size 3
        auto result = network1->propagate(wrong_size_input);
        REQUIRE(!result.has_value());
        REQUIRE(result.error() == network_error::k_invalid_input_size);
    }

    SECTION("Non-finite inputs") {
        std::vector<layer_topology> topology = {layer_topology(k_layer_size_3),
                                                layer_topology(k_layer_size_2)};
        random_generator random_gen(k_random_seed);

        auto network1 = network<float>::random(topology, random_gen);
        REQUIRE(network1.has_value());

        std::vector<float> bad_input = {k_input_1, std::numeric_limits<float>::infinity(),
                                        k_input_3};
        auto result = network1->propagate(bad_input);
        REQUIRE(!result.has_value());
        REQUIRE(result.error() == network_error::k_invalid_input_size);
    }

    SECTION("Too many weights") {
        std::vector<layer_topology> topology = {layer_topology(k_layer_size_2),
                                                layer_topology(k_layer_size_1)};

    std::vector<float> too_many_weights = {k_bias_1, k_weight_1, k_weight_2, k_weight_3,
                           k_weight_4};  // Only need 3 values (bias+2w)
        auto result = network<float>::from_weights(topology, too_many_weights);
        REQUIRE(!result.has_value());
        REQUIRE(result.error() == network_error::k_too_many_weights);
    }

    SECTION("Not enough weights") {
        std::vector<layer_topology> topology = {layer_topology(k_layer_size_2),
                                                layer_topology(k_layer_size_1)};

    std::vector<float> too_few_weights = {k_bias_1, k_weight_1};  // Need 3 values (bias+2w)
        auto result = network<float>::from_weights(topology, too_few_weights);
        REQUIRE(!result.has_value());
        REQUIRE(result.error() == network_error::k_not_enough_weights);
    }
}
