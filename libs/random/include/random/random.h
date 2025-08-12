#ifndef CSHORELARK_NEURAL_NETWORK_RANDOM_H
#define CSHORELARK_NEURAL_NETWORK_RANDOM_H

/**
 * @file random.h
 * @brief Random number generation utilities for neural networks
 */

#include <cstdint>
#include <random>
#include <stdexcept>

namespace cshorelark::random {

/**
 * @brief Random number generator for neural network weights
 */
class random_generator {
public:
    /**
     * @brief Creates a random number generator with a random seed
     */
    random_generator() : generator_(std::random_device{}()) {}

    /**
     * @brief Creates a random number generator with a specific seed
     * @param seed The seed value to use
     */
    explicit random_generator(std::uint64_t seed) : generator_(seed) {}

    /**
     * @brief Generates a random weight value
     * @return Random value between -1 and 1
     */
    [[nodiscard]] auto generate_weight() -> float {
        return default_distribution_(generator_);  // Random weight in the range [-1, 1)
    }

    /**
     * @brief Generates a random position value in the range [0, 1]
     * @return Random value between 0 and 1
     */
    [[nodiscard]] auto generate_position() -> float {
        return (default_distribution_(generator_) + 1.0F) /
               2.0F;  // Random position in the range [0, 1)
    }

    /**
     * @brief Generates a random rotation value in radians
     * @return Random rotation value
     *
     * @details The rotation is generated in the range [-π, π).
     */
    [[nodiscard]] auto generate_rotation() -> float {
        return generate_position() * 3.14159265358979323846F;  // Random rotation in radians;
    }

    /**
     * @brief Generates a random value in the specified range
     * @param min Minimum value of the range
     * @param max Maximum value of the range
     * @return Random value in the range [min, max)
     *
     * @details The generated value is uniformly distributed within the specified range.
     *
     * @throws std::invalid_argument if min is greater than max
     * @throws std::invalid_argument if min equals max, in which case min is returned
     */
    [[nodiscard]] auto generate_in_range(float min, float max) -> float {
        if (min > max) {
            throw std::invalid_argument("Minimum value must be less than maximum value.");
        }
        if (min == max) {
            return min;  // If the range is zero, return the minimum value
        }
        // Use generate_position() to get [0,1) and scale to [min, max)
        return generate_position() * (max - min) + min;
    }

    /**
     * @brief Gets the underlying random number generator
     * @return Reference to the random number generator
     */
    [[nodiscard]] auto get_engine() -> std::mt19937_64& { return generator_; }

    /**
     * @brief Seeds the random number generator
     * @param seed Seed value to use
     */
    void seed(std::uint64_t seed) { generator_.seed(seed); }

private:
    std::mt19937_64 generator_;
    std::uniform_real_distribution<float> default_distribution_ =
        std::uniform_real_distribution<float>(-1.0F, 1.0F);
};

}  // namespace cshorelark::random

#endif  // CSHORELARK_NEURAL_NETWORK_RANDOM_H_