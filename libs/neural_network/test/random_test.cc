#include "neural_network/random.h"  // NOLINT

#include <algorithm>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <vector>


using namespace cshorelark::neural_network;

TEST_CASE("Random number generator works correctly", "[random]") {
    SECTION("Generated weights are within bounds") {
        auto& rng = Random::Instance();
        std::vector<float> weights;

        // Generate a good sample size
        for (int i = 0; i < 1000; ++i) {
            weights.push_back(rng.GenerateWeight());
        }

        // All weights should be between -1 and 1
        bool all_in_range = std::all_of(weights.begin(), weights.end(),
                                        [](float w) { return w >= -1.0f && w <= 1.0f; });
        CHECK(all_in_range);
    }

    SECTION("Seeding produces deterministic results") {
        auto& rng = Random::Instance();

        // First sequence
        rng.Seed(42);
        std::vector<float> sequence1;
        for (int i = 0; i < 100; ++i) {
            sequence1.push_back(rng.GenerateWeight());
        }

        // Second sequence with same seed
        rng.Seed(42);
        std::vector<float> sequence2;
        for (int i = 0; i < 100; ++i) {
            sequence2.push_back(rng.GenerateWeight());
        }

        // Sequences should be identical
        CHECK(sequence1 == sequence2);
    }

    SECTION("Different seeds produce different sequences") {
        auto& rng = Random::Instance();

        // First sequence
        rng.Seed(42);
        std::vector<float> sequence1;
        for (int i = 0; i < 100; ++i) {
            sequence1.push_back(rng.GenerateWeight());
        }

        // Second sequence with different seed
        rng.Seed(43);
        std::vector<float> sequence2;
        for (int i = 0; i < 100; ++i) {
            sequence2.push_back(rng.GenerateWeight());
        }

        // Sequences should be different
        CHECK(sequence1 != sequence2);
    }

    SECTION("Distribution is roughly uniform") {
        auto& rng = Random::Instance();
        std::vector<float> weights;

        // Generate a large sample
        for (int i = 0; i < 10000; ++i) {
            weights.push_back(rng.GenerateWeight());
        }

        // Calculate mean (should be close to 0)
        float sum = std::accumulate(weights.begin(), weights.end(), 0.0f);
        float mean = sum / weights.size();
        CHECK_THAT(mean, Catch::Matchers::WithinAbs(0.0f, 0.1f));

        // Calculate variance (should be close to 1/3 for uniform distribution on [-1,1])
        float sum_sq =
            std::accumulate(weights.begin(), weights.end(), 0.0f,
                            [mean](float acc, float x) { return acc + (x - mean) * (x - mean); });
        float variance = sum_sq / weights.size();
        CHECK_THAT(variance, Catch::Matchers::WithinAbs(1.0f / 3.0f, 0.1f));
    }
}