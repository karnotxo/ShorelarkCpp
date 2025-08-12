#include "neural_network/neuron.h"

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <cstddef>
#include <random>
#include <vector>

#include "random/random.h"

using Catch::Matchers::WithinRel;
using cshorelark::neural_network::neuron;
using cshorelark::neural_network::neuron_error;
using cshorelark::random::random_generator;

namespace {
// Constants for test configuration
constexpr float k_weight_1 = 0.1F;
constexpr float k_weight_2 = 0.2F;
constexpr float k_weight_3 = 0.3F;
constexpr float k_weight_4 = 0.4F;
constexpr float k_weight_5 = 0.5F;
constexpr float k_weight_negative_3 = -0.3F;
constexpr float k_weight_negative_5 = -0.5F;
constexpr float k_weight_8 = 0.8F;
constexpr float k_bias_zero = 0.0F;
constexpr float k_bias_1 = 0.1F;
constexpr float k_input_1 = 1.0F;
constexpr float k_input_negative_5 = -0.5F;
constexpr float k_large_input = 1e6F;
constexpr float k_small_input = 1e-6F;
constexpr float k_expected_small_output = 3e-6F;
constexpr float k_small_margin = 1e-7F;
constexpr float k_weight_range_min = -1.0F;
constexpr float k_weight_range_max = 1.0F;
constexpr float k_input_range_min = -10.0F;
constexpr float k_input_range_max = 10.0F;
constexpr std::size_t k_input_size_3 = 3;
constexpr std::size_t k_property_test_iterations = 100;
constexpr int k_random_seed = 42;
}  // namespace

TEST_CASE("Neuron - Basic Construction", "[neuron]") {
    SECTION("Explicit construction") {
        std::vector<float> weights{k_weight_5, k_weight_negative_3, k_weight_8};
        float bias = k_bias_1;
        neuron<float> neuron1(bias, weights);

        CHECK(neuron1.weights() == weights);
        CHECK(neuron1.bias() == bias);
    }
}

TEST_CASE("Neuron - From Weights Construction", "[neuron]") {
    SECTION("Valid weights") {
        std::vector<float> weights{k_bias_1, k_weight_2, k_weight_3,
                                   k_weight_4};  // bias + 3 weights
        auto result = neuron<float>::from_weights(k_input_size_3, weights);

        REQUIRE(result.has_value());
        CHECK(result->bias() == k_bias_1);
        CHECK(result->weights() == std::vector<float>{k_weight_2, k_weight_3, k_weight_4});
    }

    SECTION("Not enough weights") {
        std::vector<float> weights{k_bias_1, k_weight_2};  // Only bias + 1 weight
        auto result = neuron<float>::from_weights(k_input_size_3, weights);

        REQUIRE(!result.has_value());
        CHECK(result.error() == neuron_error::k_invalid_weights_size);
    }
}

TEST_CASE("Neuron - Propagation", "[neuron]") {
    SECTION("Valid input size") {
        neuron<float> neuron1(k_bias_1,
                              std::vector<float>{k_weight_5, k_weight_negative_3, k_weight_8});
        std::vector<float> inputs{k_weight_5, k_input_1, k_input_negative_5};

        auto result = neuron1.propagate(inputs);
        REQUIRE(result.has_value());

        // Manual calculation using ReLU: max(0, 0.5*0.5 + -0.3*1.0 + 0.8*-0.5 + 0.1)
        // = max(0, 0.25 - 0.3 - 0.4 + 0.1) = max(0, -0.35) = 0.0
        const float expected =
            std::max(0.0F, k_weight_5 * k_weight_5 + k_weight_negative_3 * k_input_1 +
                               k_weight_8 * k_input_negative_5 + k_bias_1);
        CHECK_THAT(*result, WithinRel(expected));
    }

    SECTION("Invalid input size") {
        neuron<float> neuron1(k_bias_1, std::vector<float>{k_weight_5, k_weight_negative_3});
        std::vector<float> inputs{k_weight_5, k_input_1, k_input_negative_5};  // Too many inputs

        auto result = neuron1.propagate(inputs);
        REQUIRE(!result.has_value());
        CHECK(result.error() == neuron_error::k_invalid_input_size);
    }
}

TEST_CASE("Neuron - Random Generation", "[neuron]") {
    random_generator rng(k_random_seed);  // Fixed seed for reproducibility

    SECTION("Random weights are within bounds") {
        auto neuron1 = neuron<float>::random(rng, k_input_size_3);

        // All weights and bias should be in [-1, 1]
        CHECK(neuron1.bias() >= k_weight_range_min);
        CHECK(neuron1.bias() <= k_weight_range_max);

        for (const auto& weight : neuron1.weights()) {
            CHECK(weight >= k_weight_range_min);
            CHECK(weight <= k_weight_range_max);
        }
    }

    SECTION("Correct number of weights") {
        const std::size_t num_inputs = GENERATE(0, 1, 5, 10);
        auto neuron1 = neuron<float>::random(rng, num_inputs);
        CHECK(neuron1.weights().size() == num_inputs);
    }
}

TEST_CASE("Neuron - Property Based Testing", "[neuron][property]") {
    random_generator rng(k_random_seed);

    SECTION("Propagation output is always non-negative (ReLU)") {
        // Generate random neurons and inputs
        for (std::size_t i = 0; i < k_property_test_iterations; ++i) {
            auto neuron1 = neuron<float>::random(rng, k_input_size_3);
            std::vector<float> inputs;

            for (std::size_t j = 0; j < k_input_size_3; ++j) {
                inputs.push_back(rng.generate_in_range(k_input_range_min, k_input_range_max));
            }

            auto result = neuron1.propagate(inputs);
            REQUIRE(result.has_value());
            CHECK(*result >= 0.0F);  // ReLU ensures non-negative output
        }
    }

    SECTION("Propagation is deterministic") {
        auto neuron1 = neuron<float>::random(rng, k_input_size_3);
        std::vector<float> inputs{k_weight_5, k_weight_negative_3, k_weight_8};

        auto result1 = neuron1.propagate(inputs);
        auto result2 = neuron1.propagate(inputs);

        REQUIRE(result1.has_value());
        REQUIRE(result2.has_value());
        CHECK(*result1 == *result2);
    }
}

TEST_CASE("Neuron - Edge Cases", "[neuron]") {
    SECTION("Zero weights and bias") {
        neuron<float> neuron1(k_bias_zero, std::vector<float>(k_input_size_3, k_bias_zero));
        std::vector<float> inputs{k_input_1, k_input_1, k_input_1};

        auto result = neuron1.propagate(inputs);
        REQUIRE(result.has_value());
        CHECK_THAT(*result, WithinRel(k_bias_zero));
    }

    SECTION("Large input values") {
        neuron<float> neuron1(k_bias_zero, std::vector<float>{k_input_1, k_input_1, k_input_1});
        std::vector<float> inputs{k_large_input, k_large_input, k_large_input};

        auto result = neuron1.propagate(inputs);
        REQUIRE(result.has_value());
        // With ReLU, large positive inputs will produce large positive outputs
        CHECK(*result > 0.0F);
    }

    SECTION("Small input values") {
        neuron<float> neuron1(k_bias_zero, std::vector<float>{k_input_1, k_input_1, k_input_1});
        std::vector<float> inputs{k_small_input, k_small_input, k_small_input};

        auto result = neuron1.propagate(inputs);
        REQUIRE(result.has_value());
        CHECK_THAT(*result, Catch::Matchers::WithinAbs(k_expected_small_output, k_small_margin));
    }

    SECTION("Negative weighted sum results in zero output (ReLU)") {
        neuron<float> neuron1(k_bias_zero, std::vector<float>{-k_input_1, -k_input_1, -k_input_1});
        std::vector<float> inputs{k_input_1, k_input_1, k_input_1};

        auto result = neuron1.propagate(inputs);
        REQUIRE(result.has_value());
        // ReLU: max(0, -1*1 + -1*1 + -1*1 + 0) = max(0, -3) = 0
        CHECK_THAT(*result, WithinRel(k_bias_zero));
    }
}

TEST_CASE("Neuron - Weights round-trip", "[neuron]") {
    // Construct neuron explicitly
    std::vector<float> w = {k_weight_2, k_weight_3, k_weight_4};
    neuron<float> n(k_bias_1, w);
    auto flat = n.to_weights();  // bias first then weights
    REQUIRE(flat.size() == w.size() + 1);
    CHECK(flat[0] == k_bias_1);
    for (std::size_t i = 0; i < w.size(); ++i) {
        CHECK(flat[i + 1] == w[i]);
    }

    auto reconstructed = neuron<float>::from_weights(w.size(), flat);
    REQUIRE(reconstructed.has_value());
    CHECK(reconstructed->bias() == k_bias_1);
    REQUIRE(reconstructed->weights().size() == w.size());
    for (std::size_t i = 0; i < w.size(); ++i) {
        CHECK(reconstructed->weights()[i] == w[i]);
    }
}
