#include "simulation/statistics.h"

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <cstddef>
#include <memory>
#include <string>
#include <vector>

#include "genetic_algorithm/individual.h"
#include "genetic_algorithm/statistics.h"


using Catch::Matchers::WithinRel;

namespace {

// Test constants to avoid magic numbers
constexpr float k_test_fitness = 42.0F;
constexpr std::size_t k_test_generation = 123;
constexpr std::size_t k_test_generation2 = 7;

// Test individual class for creating statistics
class test_individual : public cshorelark::genetic::individual {
public:
    [[nodiscard]] auto get_fitness() const -> float override { return k_test_fitness; }

    [[nodiscard]] auto get_chromosome() const -> const cshorelark::genetic::chromosome& override {
        static cshorelark::genetic::chromosome chr(std::vector<float>{1.0F, 2.0F, 3.0F});
        return chr;
    }
};

// Helper to create GA statistics for testing
auto create_test_ga_stats() -> cshorelark::genetic::statistics {
    std::vector<std::unique_ptr<cshorelark::genetic::individual>> population;
    population.push_back(std::make_unique<test_individual>());

    return cshorelark::genetic::statistics::from_population(population);
}

}  // namespace

TEST_CASE("Simulation Statistics - Basic functionality", "[statistics]") {
    auto ga_stats = create_test_ga_stats();
    cshorelark::simulation::statistics stats(k_test_generation, ga_stats);

    SECTION("Generation number is preserved") { REQUIRE(stats.generation() == k_test_generation); }

    SECTION("GA statistics are accessible") {
        const auto& stored_ga_stats = stats.ga_stats();
        REQUIRE_THAT(stored_ga_stats.min_fitness(), WithinRel(ga_stats.min_fitness()));
        REQUIRE_THAT(stored_ga_stats.max_fitness(), WithinRel(ga_stats.max_fitness()));
        REQUIRE_THAT(stored_ga_stats.avg_fitness(), WithinRel(ga_stats.avg_fitness()));
        REQUIRE_THAT(stored_ga_stats.median_fitness(), WithinRel(ga_stats.median_fitness()));
    }
}

TEST_CASE("Simulation Statistics - String formatting", "[statistics]") {
    auto ga_stats = create_test_ga_stats();
    cshorelark::simulation::statistics stats(k_test_generation2, ga_stats);

    const std::string formatted = stats.to_string();
    REQUIRE(formatted.find("Generation 7") != std::string::npos);
    REQUIRE(formatted.find("42.00") != std::string::npos);  // All fitness values should be 42.0
}