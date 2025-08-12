#include "neural_network/layer.h"

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <cstddef>
#include <iterator>
#include <random>
#include <vector>

#include "neural_network/neuron.h"
#include "random/random.h"

using Catch::Matchers::WithinRel;
using cshorelark::neural_network::layer;
using cshorelark::neural_network::layer_error;
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
constexpr float k_input_1 = 1.0F;
constexpr float k_input_2 = 0.5F;
constexpr float k_input_3 = -1.0F;
constexpr float k_negative_input = -1.0F;
constexpr std::size_t k_num_inputs_2 = 2;
constexpr std::size_t k_num_inputs_3 = 3;
constexpr std::size_t k_num_neurons_2 = 2;
constexpr std::size_t k_num_neurons_3 = 3;
constexpr std::size_t k_expected_weights_6 = 6;
constexpr std::size_t k_expected_weights_8 = 8;
constexpr std::size_t k_expected_weights_13 = 13;
constexpr int k_random_seed = 42;
constexpr float k_weight_range_min = -1.0F;
constexpr float k_weight_range_max = 1.0F;
}  // namespace

TEST_CASE("Layer propagates input correctly", "[layer]") {
    // Create a layer with 2 neurons, each taking 3 inputs
    std::vector<neuron<float>> neurons;
    neurons.emplace_back(k_bias_zero, std::vector<float>{k_weight_1, k_weight_2, k_weight_3});
    neurons.emplace_back(k_bias_zero, std::vector<float>{k_weight_4, k_weight_5, k_weight_6});

    auto layer_result = layer<float>::create(std::move(neurons));
    REQUIRE(layer_result.has_value());
    const auto& test_layer = layer_result.value();

    SECTION("Propagates through all neurons") {
        const std::vector<float> inputs = {k_input_1, k_input_2, k_input_3};
        const auto outputs = test_layer.propagate(inputs);

        REQUIRE(outputs.has_value());
        REQUIRE(outputs->size() == 2);

        // Expected outputs using ReLU activation:
        // Neuron 1: max(0, 1.0*0.1 + 0.5*0.2 + -1.0*0.3) = max(0, 0.0) = 0.0
        // Neuron 2: max(0, 1.0*0.4 + 0.5*0.5 + -1.0*0.6) = max(0, 0.15) = 0.15
        const float expected1 = std::max(
            0.0F, k_input_1 * k_weight_1 + k_input_2 * k_weight_2 + k_input_3 * k_weight_3);
        const float expected2 = std::max(
            0.0F, k_input_1 * k_weight_4 + k_input_2 * k_weight_5 + k_input_3 * k_weight_6);

        CHECK_THAT((*outputs)[0], WithinRel(expected1));
        CHECK_THAT((*outputs)[1], WithinRel(expected2));
    }
}

TEST_CASE("Layer random initialization", "[layer]") {
    random_generator rng(k_random_seed);  // Fixed seed for reproducibility

    SECTION("Creates correct number of neurons") {
        const auto layer_result = layer<float>::random(rng, k_num_inputs_3, k_num_neurons_2);
        REQUIRE(layer_result.has_value());
        const auto& test_layer = layer_result.value();
        REQUIRE(test_layer.get_neurons().size() == k_num_neurons_2);
    }

    SECTION("Each neuron has correct number of weights") {
        const auto layer_result = layer<float>::random(rng, k_num_inputs_3, k_num_neurons_2);
        REQUIRE(layer_result.has_value());
        const auto& test_layer = layer_result.value();
        for (const auto& neuron : test_layer.get_neurons()) {
            REQUIRE(neuron.weights().size() == k_num_inputs_3);
        }
    }
}

TEST_CASE("Layer - Basic Construction", "[layer]") {
    SECTION("Valid construction") {
        std::vector<neuron<float>> neurons;
        neurons.emplace_back(k_bias_1, std::vector<float>{k_weight_1, k_weight_2, k_weight_3});
        neurons.emplace_back(k_bias_2, std::vector<float>{k_weight_4, k_weight_5, k_weight_6});

        auto layer_result = layer<float>::create(std::move(neurons));
        REQUIRE(layer_result.has_value());
        const auto& test_layer = layer_result.value();
        CHECK(test_layer.size() == 2);
        CHECK(test_layer.input_size() == k_num_inputs_3);
        CHECK(test_layer.weight_count() == k_expected_weights_8);  // (3+1) * 2 neurons
    }

    SECTION("Empty neurons") {
        std::vector<neuron<float>> neurons;
        auto layer_result = layer<float>::create(std::move(neurons));
        REQUIRE(!layer_result.has_value());
        CHECK(layer_result.error() == layer_error::k_empty_neurons);
    }

    SECTION("Mismatched input sizes") {
        std::vector<neuron<float>> neurons;
        neurons.emplace_back(k_bias_1, std::vector<float>{k_weight_1, k_weight_2});  // 2 inputs
        neurons.emplace_back(k_bias_2,
                             std::vector<float>{k_weight_4, k_weight_5, k_weight_6});  // 3 inputs

        auto layer_result = layer<float>::create(std::move(neurons));
        REQUIRE(!layer_result.has_value());
        CHECK(layer_result.error() == layer_error::k_mismatched_inputs);
    }
}

TEST_CASE("Layer - FromWeights Construction", "[layer]") {
    SECTION("Valid weights") {
        // Bias first, then weights for each neuron chunk (bias + inputs)
        std::vector<float> weights = {
            k_bias_1,  k_weight_1, k_weight_2, k_weight_3,  // First neuron
            k_bias_2,  k_weight_4, k_weight_5, k_weight_6   // Second neuron
        };

        auto layer_result = layer<float>::from_weights(k_num_inputs_3, k_num_neurons_2, weights);
        REQUIRE(layer_result.has_value());
        const auto& test_layer = layer_result.value();
        CHECK(test_layer.size() == k_num_neurons_2);
        CHECK(test_layer.input_size() == k_num_inputs_3);
        CHECK(test_layer.weight_count() == k_expected_weights_8);

        // Check weights were assigned correctly
        auto layer_weights = test_layer.weights();
        REQUIRE(layer_weights.size() == weights.size());
        for (std::size_t i = 0; i < weights.size(); ++i) {
            CHECK_THAT(layer_weights[i], WithinRel(weights[i]));
        }
    }

    SECTION("Not enough weights") {
    std::vector<float> weights = {k_bias_1, k_weight_1, k_weight_2};  // Only bias + 2 weights
        auto layer_result = layer<float>::from_weights(k_num_inputs_3, k_num_neurons_2, weights);
        REQUIRE(!layer_result.has_value());
        CHECK(layer_result.error() == layer_error::k_not_enough_weights);
    }
}

TEST_CASE("Layer - Random Creation", "[layer]") {
    random_generator random_gen(k_random_seed);  // Fixed seed for reproducibility

    SECTION("Creates correct structure") {
        auto layer_result = layer<float>::random(random_gen, k_num_inputs_3, k_num_neurons_2);
        REQUIRE(layer_result.has_value());
        const auto& test_layer = layer_result.value();
        CHECK(test_layer.size() == k_num_neurons_2);
        CHECK(test_layer.input_size() == k_num_inputs_3);
        CHECK(test_layer.weight_count() == k_expected_weights_8);

        // Check all weights are within [-1, 1]
        auto weights = test_layer.weights();
        for (const float weight : weights) {
            CHECK(weight >= k_weight_range_min);
            CHECK(weight <= k_weight_range_max);
        }
    }
}

TEST_CASE("Layer - Process", "[layer]") {
    SECTION("Valid input propagation") {
        std::vector<neuron<float>> neurons;
        neurons.emplace_back(k_bias_1, std::vector<float>{k_weight_1, k_weight_2});
        neurons.emplace_back(k_bias_2, std::vector<float>{k_weight_3, k_weight_4});

        auto layer_result = layer<float>::create(std::move(neurons));
        REQUIRE(layer_result.has_value());
        const auto& test_layer = layer_result.value();

        std::vector<float> inputs = {k_input_1, k_negative_input};
        auto result = test_layer.propagate(inputs);
        REQUIRE(result.has_value());
        REQUIRE(result->size() == 2);

        // Manual calculations using ReLU activation:
        // Neuron 1: max(0, 1.0*0.1 + -1.0*0.2 + 0.5) = max(0, 0.4) = 0.4
        // Neuron 2: max(0, 1.0*0.3 + -1.0*0.4 + 0.6) = max(0, 0.5) = 0.5
        const float expected1 =
            std::max(0.0F, k_input_1 * k_weight_1 + k_negative_input * k_weight_2 + k_bias_1);
        const float expected2 =
            std::max(0.0F, k_input_1 * k_weight_3 + k_negative_input * k_weight_4 + k_bias_2);

        CHECK_THAT((*result)[0], WithinRel(expected1));
        CHECK_THAT((*result)[1], WithinRel(expected2));
    }

    SECTION("Invalid input size") {
        std::vector<neuron<float>> neurons;
        neurons.emplace_back(k_bias_1, std::vector<float>{k_weight_1, k_weight_2});

        auto layer_result = layer<float>::create(std::move(neurons));
        REQUIRE(layer_result.has_value());
        const auto& test_layer = layer_result.value();

        std::vector<float> inputs = {k_input_1, k_negative_input, k_bias_1};  // Too many inputs
        auto result = test_layer.propagate(inputs);
        REQUIRE(!result.has_value());
        CHECK(result.error() == layer_error::k_invalid_input_size);
    }
}

TEST_CASE("Layer - Weight Access", "[layer]") {
    std::vector<neuron<float>> neurons;
    neurons.emplace_back(k_weight_3, std::vector<float>{k_weight_1, k_weight_2});
    neurons.emplace_back(k_weight_6, std::vector<float>{k_weight_4, k_weight_5});

    auto layer_result = layer<float>::create(std::move(neurons));
    REQUIRE(layer_result.has_value());
    const auto& test_layer = layer_result.value();

    SECTION("Weights retrieval") {
    auto weights = test_layer.weights();
    // Expect bias first in each neuron chunk
    std::vector<float> expected = {k_weight_3, k_weight_1, k_weight_2,
                       k_weight_6, k_weight_4, k_weight_5};
        REQUIRE(weights.size() == expected.size());
        for (std::size_t i = 0; i < weights.size(); ++i) {
            CHECK_THAT(weights[i], WithinRel(expected[i]));
        }
    }
}
