#include "genetic_algorithm/statistics.h"

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/generators/catch_generators_random.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <memory>
#include <vector>

#include "genetic_algorithm/chromosome.h"
#include "genetic_algorithm/individual.h"

using namespace cshorelark::genetic;
using Catch::Matchers::WithinRel;

namespace {

// Test individual that just stores a fitness value
class test_individual : public individual {
public:
    explicit test_individual(float fitness) : fitness_(fitness) {}

    // Required individual interface implementation
    [[nodiscard]] auto get_fitness() const -> float override { return fitness_; }
    [[nodiscard]] auto get_chromosome() const -> const chromosome& override {
        static const chromosome empty_chromo({0.0F});
        return empty_chromo;
    }

private:
    float fitness_;
};

// Helper function to create a test population
std::vector<std::unique_ptr<individual>> create_test_population(
    const std::vector<float>& fitness_values) {
    std::vector<std::unique_ptr<individual>> population;
    population.reserve(fitness_values.size());
    for (float fitness : fitness_values) {
        population.push_back(std::make_unique<test_individual>(fitness));
    }
    return population;
}

}  // namespace

TEST_CASE("Statistics - Even number of individuals", "[statistics]") {
    auto population = create_test_population({30.0F, 10.0F, 20.0F, 40.0F});
    auto stats = statistics::from_population(population);

    REQUIRE_THAT(stats.min_fitness(), WithinRel(10.0F));
    REQUIRE_THAT(stats.max_fitness(), WithinRel(40.0F));
    REQUIRE_THAT(stats.avg_fitness(), WithinRel((10.0F + 20.0F + 30.0F + 40.0F) / 4.0F));
    REQUIRE_THAT(stats.median_fitness(), WithinRel((20.0F + 30.0F) / 2.0F));
}

TEST_CASE("Statistics - Odd number of individuals", "[statistics]") {
    auto population = create_test_population({30.0F, 20.0F, 40.0F});
    auto stats = statistics::from_population(population);

    REQUIRE_THAT(stats.min_fitness(), WithinRel(20.0F));
    REQUIRE_THAT(stats.max_fitness(), WithinRel(40.0F));
    REQUIRE_THAT(stats.avg_fitness(), WithinRel((20.0F + 30.0F + 40.0F) / 3.0F));
    REQUIRE_THAT(stats.median_fitness(), WithinRel(30.0F));
}

TEST_CASE("Statistics - Single individual", "[statistics]") {
    auto population = create_test_population({42.0F});
    auto stats = statistics::from_population(population);

    REQUIRE_THAT(stats.min_fitness(), WithinRel(42.0F));
    REQUIRE_THAT(stats.max_fitness(), WithinRel(42.0F));
    REQUIRE_THAT(stats.avg_fitness(), WithinRel(42.0F));
    REQUIRE_THAT(stats.median_fitness(), WithinRel(42.0F));
}

TEST_CASE("Statistics - Empty population", "[statistics]") {
    std::vector<std::unique_ptr<individual>> empty_population;
    auto stats = statistics::from_population(empty_population);

    REQUIRE_THAT(stats.min_fitness(), WithinRel(0.0F));
    REQUIRE_THAT(stats.max_fitness(), WithinRel(0.0F));
    REQUIRE_THAT(stats.avg_fitness(), WithinRel(0.0F));
    REQUIRE_THAT(stats.median_fitness(), WithinRel(0.0F));
}

#ifdef NDEBUG
TEST_CASE("Statistics - Empty population throws in release", "[statistics]") {
    std::vector<test_individual> empty;
    REQUIRE_THROWS(statistics::from_population(empty));
}
#endif