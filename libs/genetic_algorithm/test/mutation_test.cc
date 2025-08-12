#include "genetic_algorithm/mutation.h"

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <cmath>
#include <numeric>
#include <vector>

#include "genetic_algorithm/chromosome.h"
#include "genetic_algorithm/genetic_error.h"
#include "random/random.h"

using namespace cshorelark::genetic;
using namespace cshorelark::random;
using Catch::Matchers::WithinRel;

namespace {
// Constants for test configuration
constexpr float k_epsilon = 1e-6F;
constexpr float k_default_mutation_range = 1.0F;
constexpr float k_default_mutation_step = 0.1F;
constexpr float k_high_mutation_rate = 0.9F;
constexpr float k_low_mutation_rate = 0.1F;
constexpr float k_limited_range = 0.5F;
constexpr float k_high_mutation_threshold = 0.7F;
constexpr float k_low_mutation_threshold = 0.3F;
constexpr float k_statistical_tolerance = 0.1F;
constexpr float k_gaussian_std_dev = 2.0F;
constexpr int k_statistical_trials = 10000;
constexpr int k_mutation_trials = 100;
constexpr int k_chromosome_size = 4;
constexpr float k_zero_gene = 0.0F;
constexpr float k_one_gene = 1.0F;
constexpr float k_two_gene = 2.0F;
constexpr float k_three_gene = 3.0F;
constexpr float k_negative_mutation_rate = -0.5F;
constexpr float k_high_mutation_rate_config = 1.5F;
constexpr float k_negative_mutation_range = -1.0F;
constexpr float k_negative_mutation_step = -0.1F;
constexpr float k_normal_mutation_rate = 0.1F;
constexpr float k_normal_mutation_step = 0.2F;

// Helper function to create a test chromosome
chromosome create_test_chromosome(const std::vector<float>& genes) { return chromosome(genes); }

// Helper function to count differences between chromosomes
std::size_t count_differences(const chromosome& chr1, const chromosome& chr2) {
    if (chr1.size() != chr2.size())
        return std::max(chr1.size(), chr2.size());

    std::size_t diff_count = 0;
    for (std::size_t i = 0; i < chr1.size(); ++i) {
        if (std::abs(chr1[i] - chr2[i]) > k_epsilon)
            ++diff_count;
    }
    return diff_count;
}

// Helper function to create a zero-filled chromosome
chromosome create_zero_chromosome() {
    return create_test_chromosome(std::vector<float>(k_chromosome_size, k_zero_gene));
}
}  // namespace

TEST_CASE("Gaussian mutation", "[mutation]") {
    random_generator rng(42);  // Fixed seed for reproducibility

    SECTION("Construction with invalid parameters") {
        CHECK_THROWS_AS(gaussian_mutation(-0.1F, k_default_mutation_range), std::invalid_argument);
        CHECK_THROWS_AS(gaussian_mutation(1.1F, k_default_mutation_range), std::invalid_argument);
        CHECK_THROWS_AS(gaussian_mutation(0.5F, 0.0F), std::invalid_argument);
        CHECK_THROWS_AS(gaussian_mutation(0.5F, -1.0F), std::invalid_argument);
    }

    SECTION("Mutation with 0% chance") {
        const gaussian_mutation mutation(0.0F, k_default_mutation_range);
        auto chromo = create_test_chromosome({k_one_gene, k_two_gene, k_three_gene});
        auto original = chromo.clone();

        auto result = mutation.mutate(chromo, rng);
        REQUIRE(result.has_value());

        // Should be unchanged
        REQUIRE(chromo.size() == 3);
        for (std::size_t i = 0; i < chromo.size(); ++i) {
            CHECK_THAT(chromo[i], WithinRel(original[i]));
        }
    }

    SECTION("Mutation with 100% chance") {
        const gaussian_mutation mutation(1.0F, k_default_mutation_range);
        auto chromo = create_test_chromosome({k_one_gene, k_one_gene, k_one_gene});
        auto original = chromo.clone();

        auto result = mutation.mutate(chromo, rng);
        REQUIRE(result.has_value());

        // All genes should be different
        bool all_changed = true;
        for (std::size_t i = 0; i < chromo.size(); ++i) {
            if (std::abs(chromo[i] - original[i]) < k_epsilon) {
                all_changed = false;
                break;
            }
        }
        CHECK(all_changed);
    }

    SECTION("Statistical properties") {
        const gaussian_mutation mutation(1.0F, k_gaussian_std_dev);  // Always mutate with std=2.0
        auto chromo = create_test_chromosome({k_zero_gene});

        // Collect many mutations to verify distribution
        std::vector<float> mutations;
        mutations.reserve(k_statistical_trials);

        for (int i = 0; i < k_statistical_trials; ++i) {
            chromo = create_test_chromosome({k_zero_gene});  // Reset
            auto result = mutation.mutate(chromo, rng);
            REQUIRE(result.has_value());
            mutations.push_back(chromo[0]);
        }

        // Calculate mean and standard deviation
        const float sum = std::accumulate(mutations.begin(), mutations.end(), 0.0F);
        const float mean = sum / k_statistical_trials;

        float sq_sum = 0.0F;
        for (float x : mutations) {
            sq_sum += (x - mean) * (x - mean);
        }
        const float std_dev = std::sqrt(sq_sum / (k_statistical_trials - 1));

        // Mean should be close to 0, std dev close to 2.0
        CHECK_THAT(mean, WithinRel(k_zero_gene, k_statistical_tolerance));
        CHECK_THAT(std_dev, WithinRel(k_gaussian_std_dev, k_statistical_tolerance));
    }

    SECTION("Mutation with high mutation rate") {
        const gaussian_mutation mutation(k_high_mutation_rate, k_default_mutation_range);

        // Run multiple trials to ensure high mutation rate
        std::size_t total_mutations = 0;
        std::size_t total_genes = 0;

        for (int i = 0; i < k_mutation_trials; ++i) {
            auto chromo = create_zero_chromosome();
            auto original = chromo.clone();
            auto result = mutation.mutate(chromo, rng);
            REQUIRE(result.has_value());

            total_mutations += count_differences(original, chromo);
            total_genes += chromo.size();
        }

        const float mutation_ratio =
            static_cast<float>(total_mutations) / static_cast<float>(total_genes);
        REQUIRE(mutation_ratio >
                k_high_mutation_threshold);  // Allow some variance due to randomness
    }

    SECTION("Mutation with low mutation rate") {
        const gaussian_mutation mutation(k_low_mutation_rate, k_default_mutation_range);

        // Run multiple trials to ensure low mutation rate
        std::size_t total_mutations = 0;
        std::size_t total_genes = 0;

        for (int i = 0; i < k_mutation_trials; ++i) {
            auto chromo = create_zero_chromosome();
            auto original = chromo.clone();
            auto result = mutation.mutate(chromo, rng);
            REQUIRE(result.has_value());

            total_mutations += count_differences(original, chromo);
            total_genes += chromo.size();
        }

        const float mutation_ratio =
            static_cast<float>(total_mutations) / static_cast<float>(total_genes);
        REQUIRE(mutation_ratio <
                k_low_mutation_threshold);  // Allow some variance due to randomness
    }

    SECTION("Mutation respects range limits") {
        const gaussian_mutation mutation(1.0F,
                                         k_limited_range);  // High mutation rate but limited range

        auto chromo = create_zero_chromosome();
        auto result = mutation.mutate(chromo, rng);
        REQUIRE(result.has_value());

        for (const float gene : chromo) {
            REQUIRE(std::abs(gene) <= k_limited_range);
        }
    }
}

TEST_CASE("Gaussian mutation parameters", "[mutation]") {
    SECTION("Parameters are clamped to valid ranges") {
        gaussian_mutation mutation1(k_negative_mutation_rate, k_default_mutation_range);
        gaussian_mutation mutation2(k_high_mutation_rate_config, k_negative_mutation_range);
        gaussian_mutation mutation3(k_normal_mutation_rate, k_default_mutation_range);

        // Test mutation with clamped parameters
        random_generator rng;
        auto chromo = create_zero_chromosome();

        auto result1 = mutation1.mutate(chromo, rng);
        REQUIRE(result1.has_value());

        auto result2 = mutation2.mutate(chromo, rng);
        REQUIRE(result2.has_value());

        auto result3 = mutation3.mutate(chromo, rng);
        REQUIRE(result3.has_value());
    }
}

TEST_CASE("Gaussian mutation with default parameters", "[mutation]") {
    random_generator rng;

    SECTION("Mutation with default configuration") {
        gaussian_mutation mutation;
        auto chromosome = create_zero_chromosome();
        auto original = chromosome.clone();

        auto result = mutation.mutate(chromosome, rng);
        REQUIRE(result.has_value());

        // Some genes should be different due to mutation
        std::size_t diff_count = count_differences(original, chromosome);
        REQUIRE(diff_count > 0);

        // Mutations should be bounded
        for (const float gene : chromosome) {
            REQUIRE(std::abs(gene) <= k_default_mutation_range);
        }
    }

    SECTION("Mutation with high mutation rate") {
        const gaussian_mutation mutation(k_high_mutation_rate, k_default_mutation_range);

        // Run multiple trials to ensure high mutation rate
        std::size_t total_mutations = 0;
        std::size_t total_genes = 0;

        for (int i = 0; i < k_mutation_trials; ++i) {
            auto chromosome = create_zero_chromosome();
            auto original = chromosome.clone();
            auto result = mutation.mutate(chromosome, rng);
            REQUIRE(result.has_value());

            total_mutations += count_differences(original, chromosome);
            total_genes += chromosome.size();
        }

        const float mutation_ratio =
            static_cast<float>(total_mutations) / static_cast<float>(total_genes);
        REQUIRE(mutation_ratio >
                k_high_mutation_threshold);  // Allow some variance due to randomness
    }

    SECTION("Mutation with low mutation rate") {
        gaussian_mutation mutation(k_low_mutation_rate, k_default_mutation_range);

        // Run multiple trials to ensure low mutation rate
        std::size_t total_mutations = 0;
        std::size_t total_genes = 0;

        for (int i = 0; i < k_mutation_trials; ++i) {
            auto chromosome = create_zero_chromosome();
            auto original = chromosome.clone();
            auto result = mutation.mutate(chromosome, rng);
            REQUIRE(result.has_value());

            total_mutations += count_differences(original, chromosome);
            total_genes += chromosome.size();
        }

        const float mutation_ratio =
            static_cast<float>(total_mutations) / static_cast<float>(total_genes);
        REQUIRE(mutation_ratio <
                k_low_mutation_threshold);  // Allow some variance due to randomness
    }

    SECTION("Mutation with limited range") {
        const gaussian_mutation mutation(1.0F,
                                         k_limited_range);  // High mutation rate but limited range

        auto chromo = create_zero_chromosome();
        auto result = mutation.mutate(chromo, rng);
        REQUIRE(result.has_value());

        for (const float gene : chromo) {
            REQUIRE(std::abs(gene) <= k_limited_range);
        }
    }
}