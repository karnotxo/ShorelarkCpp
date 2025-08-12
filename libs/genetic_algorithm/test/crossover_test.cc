#include "genetic_algorithm/crossover.h"

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <vector>

#include "random/random.h"

using cshorelark::genetic::chromosome;
using cshorelark::genetic::genetic_error_code;
using cshorelark::genetic::single_point_crossover;
using cshorelark::genetic::uniform_crossover;
using cshorelark::random::random_generator;

namespace {
// Helper function to create a test chromosome
chromosome create_test_chromosome(const std::vector<float>& genes) { return chromosome(genes); }
}  // namespace

TEST_CASE("Single point crossover", "[crossover]") {
    random_generator rand(42);  // Fixed seed for reproducibility
    single_point_crossover crossover;
    SECTION("Crossover with equal length parents") {
        auto parent1 = create_test_chromosome({1.0F, 2.0F, 3.0F, 4.0F});
        auto parent2 = create_test_chromosome({5.0F, 6.0F, 7.0F, 8.0F});

        auto result = crossover.crossover(parent1, parent2, rand);
        REQUIRE(result.has_value());

        const auto& child = result.value();
        REQUIRE(child.size() == parent1.size());

        // Child should contain genes from both parents
        bool has_parent1_genes = false;
        bool has_parent2_genes = false;

        for (std::size_t i = 0; i < child.size(); ++i) {
            if (child[i] == parent1[i])
                has_parent1_genes = true;
            if (child[i] == parent2[i])
                has_parent2_genes = true;
        }

        REQUIRE(has_parent1_genes);
        REQUIRE(has_parent2_genes);
    }
    SECTION("Crossover fails with different length parents") {
        auto parent1 = create_test_chromosome({1.0F, 2.0F, 3.0F});
        auto parent2 = create_test_chromosome({5.0F, 6.0F, 7.0F, 8.0F});

        auto result = crossover.crossover(parent1, parent2, rand);
        REQUIRE(!result.has_value());
        REQUIRE(result.error().code == genetic_error_code::k_invalid_parent_size);
    }
}

// Removed test case for uniform crossover configuration as it's no longer applicable

TEST_CASE("Uniform crossover", "[crossover]") {
    random_generator rand(42);  // Fixed seed for reproducibility

    SECTION("Crossover with default configuration") {
        uniform_crossover crossover;
        auto parent1 = create_test_chromosome({1.0F, 2.0F, 3.0F, 4.0F});
        auto parent2 = create_test_chromosome({5.0F, 6.0F, 7.0F, 8.0F});

        auto result = crossover.crossover(parent1, parent2, rand);
        REQUIRE(result.has_value());

        const auto& child = result.value();
        REQUIRE(child.size() == parent1.size());

        // Child should contain genes from both parents
        bool has_parent1_genes = false;
        bool has_parent2_genes = false;

        for (std::size_t i = 0; i < child.size(); ++i) {
            if (child[i] == parent1[i])
                has_parent1_genes = true;
            if (child[i] == parent2[i])
                has_parent2_genes = true;
        }

        REQUIRE(has_parent1_genes);
        REQUIRE(has_parent2_genes);
    }
    SECTION("Crossover with high swap probability") {
        uniform_crossover crossover(0.9F);

        auto parent1 = create_test_chromosome({1.0F, 2.0F, 3.0F, 4.0F});
        auto parent2 = create_test_chromosome(
            {5.0F, 6.0F, 7.0F, 8.0F});  // Run multiple trials to ensure high swap rate
        int parent1_genes = 0;
        int total_genes = 0;

        for (int i = 0; i < 100; ++i) {
            auto result = crossover.crossover(parent1, parent2, rand);
            REQUIRE(result.has_value());

            const auto& child = result.value();
            for (std::size_t j = 0; j < child.size(); ++j) {
                if (child[j] == parent1[j])
                    ++parent1_genes;
                ++total_genes;
            }
        }

        // With high swap probability, we expect most genes to come from parent1
        float parent1_ratio = static_cast<float>(parent1_genes) / total_genes;
        REQUIRE(parent1_ratio > 0.7F);  // Allow some variance due to randomness
    }
    SECTION("Crossover with low swap probability") {
        uniform_crossover crossover(0.1F);

        auto parent1 = create_test_chromosome({1.0F, 2.0F, 3.0F, 4.0F});
        auto parent2 = create_test_chromosome(
            {5.0F, 6.0F, 7.0F, 8.0F});  // Run multiple trials to ensure low swap rate
        int parent1_genes = 0;
        int total_genes = 0;

        for (int i = 0; i < 100; ++i) {
            auto result = crossover.crossover(parent1, parent2, rand);
            REQUIRE(result.has_value());

            const auto& child = result.value();
            for (std::size_t j = 0; j < child.size(); ++j) {
                if (child[j] == parent1[j])
                    ++parent1_genes;
                ++total_genes;
            }
        }

        // With low swap probability, we expect few genes to come from parent1
        float parent1_ratio = static_cast<float>(parent1_genes) / total_genes;
        REQUIRE(parent1_ratio < 0.3F);  // Allow some variance due to randomness
    }
}