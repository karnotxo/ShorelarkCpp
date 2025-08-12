#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <optimizer/analyze.hpp>


using namespace shorelark;

TEST_CASE("Analyze can compute statistics", "[analyze]") {
    std::vector<float> fitnesses = {1.0f, 2.0f, 3.0f, 4.0f, 5.0f};

    auto stats = Analyze::compute_stats(fitnesses);

    CHECK(stats.min_fitness == 1.0f);
    CHECK(stats.max_fitness == 5.0f);
    CHECK(stats.avg_fitness == Catch::Approx(3.0f));
    CHECK(stats.median_fitness == 3.0f);
}

TEST_CASE("Analyze computes correct median for even number of values",
          "[analyze]") {
    std::vector<float> fitnesses = {1.0f, 2.0f, 3.0f, 4.0f};

    auto stats = Analyze::compute_stats(fitnesses);

    CHECK(stats.median_fitness == Catch::Approx(2.5f));
}

TEST_CASE("Analyze handles empty fitness vector", "[analyze]") {
    std::vector<float> fitnesses;

    auto stats = Analyze::compute_stats(fitnesses);

    CHECK(stats.min_fitness == 0.0f);
    CHECK(stats.max_fitness == 0.0f);
    CHECK(stats.avg_fitness == 0.0f);
    CHECK(stats.median_fitness == 0.0f);
}

TEST_CASE("Analyze can find best individual", "[analyze]") {
    std::vector<float> fitnesses = {1.0f, 5.0f, 3.0f, 2.0f, 4.0f};

    auto best_idx = Analyze::find_best_individual_idx(fitnesses);

    CHECK(best_idx == 1);   // Index of 5.0f
}

TEST_CASE("Analyze handles empty vector for best individual", "[analyze]") {
    std::vector<float> fitnesses;

    auto best_idx = Analyze::find_best_individual_idx(fitnesses);

    CHECK(best_idx == 0);
}

TEST_CASE("Analyze handles unsorted input correctly", "[analyze]") {
    std::vector<float> fitnesses = {5.0f, 1.0f, 4.0f, 2.0f, 3.0f};

    auto stats = Analyze::compute_stats(fitnesses);

    CHECK(stats.min_fitness == 1.0f);
    CHECK(stats.max_fitness == 5.0f);
    CHECK(stats.avg_fitness == Catch::Approx(3.0f));
    CHECK(stats.median_fitness == 3.0f);
}