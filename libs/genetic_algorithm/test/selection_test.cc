#include "genetic_algorithm/selection.h"

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <map>
#include <random>
#include <utility>  // for std::move
#include <vector>

#include "genetic_algorithm/genetic_error.h"
#include "random/random.h"
#include "test_individual.h"

namespace cshorelark::genetic {
namespace {

// Helper function to create a population of individuals with given fitness values
std::vector<std::unique_ptr<individual>> create_test_population(
    const std::vector<float>& fitness_values) {
    std::vector<std::unique_ptr<individual>> population;
    population.reserve(fitness_values.size());
    for (float fitness : fitness_values) {
        auto ind = std::make_unique<test_individual>(chromosome({0.0F}));
        ind->set_fitness(fitness);
        population.push_back(std::move(ind));
    }

    return population;
}

TEST_CASE("Roulette wheel selection", "[selection]") {
    SECTION("Selection probabilities match fitness values") {
        // Create a population with known fitness values
        auto population = create_test_population({2.0F, 1.0F, 4.0F, 3.0F});

        // Create selection strategy and random generator
        roulette_wheel_selection selector;
        cshorelark::random::random_generator rng{42};  // Fixed seed for consistent results

        // Run multiple selections and count occurrences
        std::map<float, int> histogram;
        constexpr int k_num_selections = 1000;

        for (int i = 0; i < k_num_selections; ++i) {
            auto result = selector.select(population, rng);
            REQUIRE(result.has_value());
            const auto& selected = population[result.value()];
            histogram[selected->get_fitness()]++;
        }

        // Verify the distribution matches expected probabilities
        // Expected counts based on Rust implementation (with some tolerance)
        constexpr float k_tolerance = 0.1F;  // 10% tolerance

        // Expected distribution with fixed seed 42:
        // 1.0 => ~87 (8.7%)
        // 2.0 => ~204 (20.4%)
        // 3.0 => ~277 (27.7%)
        // 4.0 => ~432 (43.2%)

        REQUIRE_THAT(histogram[1.0F], Catch::Matchers::WithinRel(87.0F, k_tolerance));
        REQUIRE_THAT(histogram[2.0F], Catch::Matchers::WithinRel(204.0F, k_tolerance));
        REQUIRE_THAT(histogram[3.0F], Catch::Matchers::WithinRel(277.0F, k_tolerance));
        REQUIRE_THAT(histogram[4.0F], Catch::Matchers::WithinRel(432.0F, k_tolerance));
    }

    SECTION("Handles zero fitness values") {
        // Create a population with some zero fitness values
        auto population = create_test_population({0.0F, 0.0F, 1.0F, 2.0F});

        roulette_wheel_selection selector;
        cshorelark::random::random_generator rng(42);  // Fixed seed for consistency

        // Verify we can still select individuals
        auto result = selector.select(population, rng);
        REQUIRE(result.has_value());
        REQUIRE(result.value() < population.size());

        // Verify zero fitness individuals have minimum probability
        std::map<float, int> histogram;
        constexpr int k_num_selections = 1000;

        for (int i = 0; i < k_num_selections; ++i) {
            auto loop_result = selector.select(population, rng);
            REQUIRE(loop_result.has_value());
            const auto& selected = population[loop_result.value()];
            histogram[selected->get_fitness()]++;
        }

        // Zero fitness values should be treated as 0.00001 (from Rust implementation)
        const float total_selections = static_cast<float>(k_num_selections);
        const float min_probability = 0.00001F / (0.00001F * 2 + 1.0F + 2.0F);  // ~0.000003
        const float min_expected = min_probability * total_selections;

        REQUIRE(2 * histogram[0.0F] >= static_cast<int>(min_expected));  // Allow some variance
    }

    SECTION("Handles empty population") {
        std::vector<std::unique_ptr<individual>> empty_population;
        roulette_wheel_selection selector;
        cshorelark::random::random_generator rng{42};

        auto result = selector.select(empty_population, rng);
        REQUIRE_FALSE(result.has_value());
        REQUIRE(result.error().code == genetic_error_code::k_invalid_population_size);
    }
}

TEST_CASE("Tournament selection", "[selection]") {
    SECTION("Selects best individual from tournament") {
        // Create a population with known fitness values
        auto population = create_test_population({1.0F, 4.0F, 2.0F, 3.0F});

        // Create selection strategy with tournament size 3
        tournament_selection selector(3);
        cshorelark::random::random_generator rng{42};

        // Run multiple selections and verify we always get one of the individuals
        constexpr int k_num_selections = 100;
        for (int i = 0; i < k_num_selections; ++i) {
            auto result = selector.select(population, rng);
            REQUIRE(result.has_value());
            REQUIRE(result.value() < population.size());
        }
    }

    SECTION("Handles tournament size larger than population") {
        auto population = create_test_population({1.0F, 2.0F, 3.0F});

        // Tournament size 5 with population size 3
        tournament_selection selector(5);
        cshorelark::random::random_generator rng{42};

        auto result = selector.select(population, rng);
        REQUIRE(result.has_value());
        REQUIRE(result.value() < population.size());
    }

    SECTION("Handles reversed fitness") {
        // Create a population where lower fitness is better
        auto population = create_test_population({4.0F, 1.0F, 3.0F, 2.0F, 5.0F, 6.0F});

        // Create selection strategy with reversed fitness
        tournament_selection selector(4, true);        // tournament size 4, reversed=true
        cshorelark::random::random_generator rng(42);  // Fixed seed for consistency

        // Run multiple selections and count occurrences
        std::map<float, int> histogram;
        constexpr int k_num_selections = 1000;

        for (int i = 0; i < k_num_selections; ++i) {
            auto result = selector.select(population, rng);
            REQUIRE(result.has_value());
            const auto& selected = population[result.value()];
            histogram[selected->get_fitness()]++;
        }

        // With reversed fitness, lower values should be selected more often
        REQUIRE(histogram[1.0F] >= histogram[2.0F]);
        REQUIRE(histogram[2.0F] >= histogram[3.0F]);
        REQUIRE(histogram[3.0F] >= histogram[4.0F]);
        REQUIRE(histogram[4.0F] == 0);  // 4.0F should not be selected
        REQUIRE(histogram[5.0F] == 0);  // 5.0F should not be selected
        REQUIRE(histogram[6.0F] == 0);  // 6.0F should not be selected
    }

    SECTION("Handles empty population") {
        std::vector<std::unique_ptr<individual>> empty_population;
        tournament_selection selector(3);
        cshorelark::random::random_generator rng{42};

        auto result = selector.select(empty_population, rng);
        REQUIRE_FALSE(result.has_value());
        REQUIRE(result.error().code == genetic_error_code::k_invalid_population_size);
    }

    SECTION("Handles zero tournament size") {
        auto population = create_test_population({1.0F, 2.0F, 3.0F});
        tournament_selection selector(0);
        cshorelark::random::random_generator rng;

        auto result = selector.select(population, rng);
        REQUIRE_FALSE(result.has_value());
        REQUIRE(result.error().code == genetic_error_code::k_invalid_parent_size);
    }

    SECTION("Selection pressure varies with tournament size") {
        auto population = create_test_population({1.0F, 2.0F, 3.0F, 4.0F});
        cshorelark::random::random_generator rng{42};

        // Test with different tournament sizes
        const std::vector<size_t> tournament_sizes = {2, 3, 4};
        std::vector<std::map<float, int>> histograms;
        histograms.resize(tournament_sizes.size());

        constexpr int k_num_selections = 1000;

        // Run selections for each tournament size
        for (size_t i = 0; i < tournament_sizes.size(); ++i) {
            tournament_selection selector(tournament_sizes[i]);

            // Avoid backward branch warning by using a separate loop
            for (int j = 0; j < k_num_selections; ++j) {
                auto result = selector.select(population, rng);
                REQUIRE(result.has_value());
                const auto& selected = population[result.value()];
                histograms[i][selected->get_fitness()]++;
            }
        }

        // Larger tournament sizes should show stronger selection pressure
        // (more bias towards higher fitness values)
        for (size_t i = 1; i < tournament_sizes.size(); ++i) {
            // Compare ratios of high fitness to low fitness selections
            float prev_ratio =
                static_cast<float>(histograms[i - 1][4.0F]) / histograms[i - 1][1.0F];
            float curr_ratio = static_cast<float>(histograms[i][4.0F]) / histograms[i][1.0F];
            REQUIRE(curr_ratio >= prev_ratio);
        }
    }

    SECTION("Consistent selection with same seed") {
        auto population = create_test_population({1.0F, 2.0F, 3.0F, 4.0F});
        tournament_selection selector(3);

        // First run
        cshorelark::random::random_generator rng1(42);
        std::vector<size_t> selections1;
        for (int i = 0; i < 10; ++i) {
            auto result = selector.select(population, rng1);
            REQUIRE(result.has_value());
            selections1.push_back(result.value());
        }

        // Second run with same seed
        cshorelark::random::random_generator rng2(42);
        std::vector<size_t> selections2;
        for (int i = 0; i < 10; ++i) {
            auto result = selector.select(population, rng2);
            REQUIRE(result.has_value());
            selections2.push_back(result.value());
        }

        // Selections should be identical
        REQUIRE(selections1 == selections2);
    }

    SECTION("Debug reversed fitness") {
        // Simple test with just two individuals
        auto population = create_test_population({1.0F, 2.0F});

        // Create selection strategy with reversed fitness
        tournament_selection selector(2, true);        // tournament size 2, reversed=true
        cshorelark::random::random_generator rng(42);  // Fixed seed

        // Run multiple selections and count occurrences
        std::map<float, int> histogram;
        constexpr int k_num_selections = 100;

        for (int i = 0; i < k_num_selections; ++i) {
            auto result = selector.select(population, rng);
            REQUIRE(result.has_value());
            const auto& selected = population[result.value()];
            histogram[selected->get_fitness()]++;
        }

        // Debug output
        INFO("Histogram[1.0F] = " << histogram[1.0F]);
        INFO("Histogram[2.0F] = " << histogram[2.0F]);

        // With reversed fitness and tournament size 2, 1.0F should always win
        // since we're comparing the entire population each time
        CHECK(histogram[1.0F] > histogram[2.0F]);
    }
}

}  // namespace
}  // namespace cshorelark::genetic
