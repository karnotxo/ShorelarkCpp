#ifndef CSHORELARK_NEURAL_NETWORK_ACTIVATION_H
#define CSHORELARK_NEURAL_NETWORK_ACTIVATION_H

/**
 * @file activation.h
 * @brief Neural network activation functions
 */

#include <cmath>

namespace cshorelark::neural_network {

/**
 * @brief Available activation functions for neurons
 */
enum class activation_function {
    k_sigmoid,  ///< Sigmoid activation: f(x) = 1 / (1 + e^-x)
    k_relu,     ///< Rectified Linear Unit: f(x) = max(0, x)
    k_tanh      ///< Hyperbolic tangent: f(x) = tanh(x)
};

/**
 * @brief Activation function implementations
 */
class activation {
public:
    /**
     * @brief Applies sigmoid activation function
     * @param x Input value
     * @return Sigmoid of x
     */
    static auto sigmoid(float x) -> float { return 1.0F / (1.0F + std::exp(-x)); }

    /**
     * @brief Applies ReLU activation function
     * @param x Input value
     * @return ReLU of x
     */
    static auto relu(float x) -> float { return std::fmax(0.0F, x); }

    /**
     * @brief Applies tanh activation function
     * @param x Input value
     * @return Tanh of x
     */
    static auto tanh(float x) -> float { return std::tanh(x); }

    /**
     * @brief Applies specified activation function
     * @param func Activation function to use
     * @param x Input value
     * @return Activated value
     */
    static auto apply(activation_function func, float x) -> float {
        switch (func) {
            case activation_function::k_sigmoid:
                return sigmoid(x);
            case activation_function::k_relu:
                return relu(x);
            case activation_function::k_tanh:
                return tanh(x);
            default:
                return x;  // Identity function as fallback
        }
    }
};

}  // namespace cshorelark::neural_network

#endif  // CSHORELARK_NEURAL_NETWORK_ACTIVATION_H_