#include "neural_network/activation.h"  // NOLINT

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>


using namespace cshorelark::neural_network;
using Catch::Matchers::WithinAbs;

TEST_CASE("Activation functions behave correctly", "[activation]") {
    SECTION("Sigmoid activation") {
        // Test sigmoid properties
        CHECK_THAT(activation::sigmoid(0.0f), WithinAbs(0.5f, 1e-6f));
        CHECK_THAT(activation::sigmoid(10.0f), WithinAbs(1.0f, 1e-4f));
        CHECK_THAT(activation::sigmoid(-10.0f), WithinAbs(0.0f, 1e-4f));

        // Test symmetry around 0
    float x = 2.0f;
    CHECK_THAT(activation::sigmoid(x) + activation::sigmoid(-x), WithinAbs(1.0f, 1e-6f));
    }

    SECTION("ReLU activation") {
        // Test positive values
    CHECK_THAT(activation::relu(1.0f), WithinAbs(1.0f, 1e-6f));
    CHECK_THAT(activation::relu(5.0f), WithinAbs(5.0f, 1e-6f));

        // Test negative values
    CHECK_THAT(activation::relu(-1.0f), WithinAbs(0.0f, 1e-6f));
    CHECK_THAT(activation::relu(-5.0f), WithinAbs(0.0f, 1e-6f));

        // Test zero
    CHECK_THAT(activation::relu(0.0f), WithinAbs(0.0f, 1e-6f));
    }

    SECTION("Tanh activation") {
        // Test tanh properties
    CHECK_THAT(activation::tanh(0.0f), WithinAbs(0.0f, 1e-6f));
    CHECK_THAT(activation::tanh(10.0f), WithinAbs(1.0f, 1e-4f));
    CHECK_THAT(activation::tanh(-10.0f), WithinAbs(-1.0f, 1e-4f));

        // Test odd function property
    float x = 2.0f;
    CHECK_THAT(activation::tanh(-x), WithinAbs(-activation::tanh(x), 1e-6f));
    }

    SECTION("Apply function works correctly") {
        float x = 2.0f;

        // Test each activation type
    CHECK_THAT(activation::apply(activation_function::k_sigmoid, x),
           WithinAbs(activation::sigmoid(x), 1e-6f));

    CHECK_THAT(activation::apply(activation_function::k_relu, x),
           WithinAbs(activation::relu(x), 1e-6f));

    CHECK_THAT(activation::apply(activation_function::k_tanh, x),
           WithinAbs(activation::tanh(x), 1e-6f));
    }
}
