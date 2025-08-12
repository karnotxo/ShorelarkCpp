#include "genetic_algorithm/individual.h"

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <stdexcept>
#include <vector>

#include "genetic_algorithm/chromosome.h"
#include "genetic_algorithm/individual.h"
#include "test_individual.h"

using Catch::Matchers::WithinRel;
using cshorelark::genetic::chromosome;
using cshorelark::genetic::individual;
using cshorelark::genetic::test_individual;

namespace {
// Constants for test configuration
constexpr float k_gene_1 = 1.0F;
constexpr float k_gene_2 = 2.0F;
constexpr float k_gene_3 = 3.0F;
constexpr float k_gene_4 = 4.0F;
constexpr float k_gene_5 = 5.0F;
constexpr float k_gene_6 = 6.0F;
constexpr float k_fitness_1 = 1.0F;
constexpr float k_fitness_2 = 2.0F;
constexpr float k_fitness_42 = 42.0F;
constexpr float k_gene_3_1 = 3.1F;
}  // namespace

TEST_CASE("test_individual", "[genetic]") {
    SECTION("construction and basic operations") {
        std::vector<float> genes = {k_gene_1, k_gene_2, k_gene_3};
        test_individual individ{chromosome(genes)};

        CHECK_FALSE(individ.has_fitness());
        REQUIRE_THROWS_AS(individ.get_fitness(), std::runtime_error);

        // Check chromosome access
        CHECK(individ.get_chromosome().size() == 3);
        CHECK_THAT(individ.get_chromosome()[0], WithinRel(k_gene_1));
        CHECK_THAT(individ.get_chromosome()[1], WithinRel(k_gene_2));
        CHECK_THAT(individ.get_chromosome()[2], WithinRel(k_gene_3));
    }
    SECTION("fitness handling") {
        std::vector<float> genes = {k_gene_1, k_gene_2, k_gene_3};
        test_individual individ{chromosome(genes)};

        individ.set_fitness(k_fitness_42);
        CHECK(individ.has_fitness());
        CHECK_THAT(individ.get_fitness(), WithinRel(k_fitness_42));
    }
    SECTION("factory method") {
        std::vector<float> genes = {k_gene_1, k_gene_2, k_gene_3};
        auto individ = test_individual::create(genes);

        CHECK_FALSE(individ.has_fitness());
        CHECK(individ.get_chromosome().size() == 3);
        CHECK_THAT(individ.get_chromosome()[0], WithinRel(k_gene_1));
    }
    SECTION("comparison") {
        const std::vector<float> genes = {k_gene_1, k_gene_2, k_gene_3};
        test_individual a{chromosome(genes)};
        test_individual b{chromosome(genes)};

        // Without fitness, comparison through the individual interface should throw
        // We need to access through the individual interface to test the behavior
        individual& a_ref = a;
        individual& b_ref = b;
        REQUIRE_THROWS_AS(a_ref.get_fitness() < b_ref.get_fitness(), std::runtime_error);

        // With fitness, comparison should work
        a.set_fitness(k_fitness_1);
        b.set_fitness(k_fitness_2);
        CHECK(a.get_fitness() < b.get_fitness());
        CHECK(b.get_fitness() > a.get_fitness());

        // Equal fitness
        b.set_fitness(k_fitness_1);
        CHECK(a.get_fitness() == b.get_fitness());
    }
    SECTION("equality") {
        std::vector<float> genes = {k_gene_1, k_gene_2, k_gene_3};
        test_individual a{chromosome(genes)};
        test_individual b{chromosome(genes)};
        test_individual c{
            chromosome({k_gene_1, k_gene_2,
                        k_gene_3_1})};  // Ensure chromosomes are comparable via custom comparison
        // Note: We can compare references since we want to test chromosome equality
        CHECK(std::equal(a.get_chromosome().begin(), a.get_chromosome().end(),
                         b.get_chromosome().begin(), b.get_chromosome().end()));
        CHECK_FALSE(std::equal(a.get_chromosome().begin(), a.get_chromosome().end(),
                         c.get_chromosome().begin(), c.get_chromosome().end()));

        // Same chromosomes, different fitness
        a.set_fitness(k_fitness_1);
        CHECK(a.has_fitness());
        CHECK_FALSE(b.has_fitness());  // b has no fitness

        b.set_fitness(k_fitness_2);
        CHECK(a.get_fitness() != b.get_fitness());  // different fitness

        b.set_fitness(k_fitness_1);
        CHECK(a.get_fitness() == b.get_fitness());  // same fitness
    }
    SECTION("move semantics") {
        std::vector<float> genes = {k_gene_1, k_gene_2, k_gene_3};
        test_individual a{chromosome(genes)};
        a.set_fitness(k_fitness_42);

        // Move construction
        test_individual b{std::move(a)};
        CHECK(b.has_fitness());
        CHECK_THAT(b.get_fitness(), WithinRel(k_fitness_42));

        // Move assignment
        test_individual ind_c{chromosome(genes)};
        ind_c = std::move(b);
        CHECK(ind_c.has_fitness());
        CHECK_THAT(ind_c.get_fitness(), WithinRel(k_fitness_42));
    }
    SECTION("cloning") {
        std::vector<float> genes = {k_gene_1, k_gene_2, k_gene_3};
        test_individual original{chromosome(genes)};
        original.set_fitness(k_fitness_42);

        auto cloned = original.clone();
        REQUIRE(cloned != nullptr);
        CHECK(cloned->has_fitness());
        CHECK_THAT(cloned->get_fitness(), WithinRel(k_fitness_42));
        CHECK(cloned->get_chromosome().size() == 3);
        CHECK_THAT(cloned->get_chromosome()[0], WithinRel(k_gene_1));
        CHECK_THAT(cloned->get_chromosome()[1], WithinRel(k_gene_2));
        CHECK_THAT(cloned->get_chromosome()[2], WithinRel(k_gene_3));
    }
    SECTION("offspring creation") {
        std::vector<float> genes = {k_gene_1, k_gene_2, k_gene_3};
        test_individual parent{chromosome(genes)};
        parent.set_fitness(k_fitness_42);
        std::vector<float> offspring_genes = {k_gene_4, k_gene_5, k_gene_6};
        auto result = parent.create_offspring(chromosome(offspring_genes));
        REQUIRE(result.has_value());

        auto& offspring = *result;
        CHECK_FALSE(offspring->has_fitness());
        CHECK(offspring->get_chromosome().size() == 3);
        CHECK_THAT(offspring->get_chromosome()[0], WithinRel(k_gene_4));
        CHECK_THAT(offspring->get_chromosome()[1], WithinRel(k_gene_5));
        CHECK_THAT(offspring->get_chromosome()[2], WithinRel(k_gene_6));
    }
}
