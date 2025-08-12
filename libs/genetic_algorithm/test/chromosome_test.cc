#include "genetic_algorithm/chromosome.h"

#include <algorithm>
#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/generators/catch_generators_random.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <iterator>  // For std::back_inserter
#include <numeric>
#include <vector>

#include "range/v3/algorithm/copy.hpp"
#include "range/v3/algorithm/for_each.hpp"
#include "range/v3/range.hpp"
#include "test_individual.h"

using cshorelark::genetic::chromosome;
using cshorelark::genetic::chromosome_error;
using cshorelark::genetic::test_individual;

TEST_CASE("Chromosome", "[genetic]") {
    SECTION("default construction") {
        chromosome chromo{
            std::vector<float>()  // Empty chromosome
        };
        CHECK(chromo.empty());
    }
    SECTION("construction from initializer list") {
        chromosome chromo(std::vector<float>{3.0f, 1.0f, 2.0f});

        CHECK(chromo.size() == 3);
        CHECK_FALSE(chromo.empty());

        CHECK_THAT(chromo[0], Catch::Matchers::WithinRel(3.0f));
        CHECK_THAT(chromo[1], Catch::Matchers::WithinRel(1.0f));
        CHECK_THAT(chromo[2], Catch::Matchers::WithinRel(2.0f));
    }

    SECTION("construction from range") {
        std::vector<float> genes = {3.0f, 1.0f, 2.0f};
        chromosome chromo(genes);

        CHECK(chromo.size() == 3);
        CHECK_FALSE(chromo.empty());

        CHECK_THAT(chromo[0], Catch::Matchers::WithinRel(3.0f));
        CHECK_THAT(chromo[1], Catch::Matchers::WithinRel(1.0f));
        CHECK_THAT(chromo[2], Catch::Matchers::WithinRel(2.0f));
    }

    SECTION("factory methods") {
        SECTION("from_range") {
            std::vector<float> genes = {3.0f, 1.0f, 2.0f};
            auto chromo = chromosome::from_range(genes.begin(), genes.end());

            CHECK(chromo.size() == 3);
            CHECK_THAT(chromo[0], Catch::Matchers::WithinRel(3.0f));
            CHECK_THAT(chromo[1], Catch::Matchers::WithinRel(1.0f));
            CHECK_THAT(chromo[2], Catch::Matchers::WithinRel(2.0f));
        }
    }

    SECTION("iteration") {
        std::vector<float> genes = {3.0f, 1.0f, 2.0f};
        const chromosome chromo(genes);

        std::vector<float> collected;
        std::copy(chromo.begin(), chromo.end(), std::back_inserter(collected));

        REQUIRE(collected.size() == 3);
        CHECK_THAT(collected[0], Catch::Matchers::WithinRel(3.0f));
        CHECK_THAT(collected[1], Catch::Matchers::WithinRel(1.0f));
        CHECK_THAT(collected[2], Catch::Matchers::WithinRel(2.0f));
    }

    SECTION("mutable iteration") {
        const std::vector<float> genes = {3.0f, 1.0f, 2.0f};
        chromosome chromo(genes);  // Multiply each gene by 10
        std::for_each(chromo.begin(), chromo.end(), [](float& gene) { gene *= 10.0f; });

        std::vector<float> collected;
        std::copy(chromo.begin(), chromo.end(), std::back_inserter(collected));

        REQUIRE(collected.size() == 3);
        CHECK_THAT(collected[0], Catch::Matchers::WithinRel(30.0f));
        CHECK_THAT(collected[1], Catch::Matchers::WithinRel(10.0f));
        CHECK_THAT(collected[2], Catch::Matchers::WithinRel(20.0f));
    }

    SECTION("indexing and bounds checking") {
        std::vector<float> genes = {3.0f, 1.0f, 2.0f};
        const chromosome chromo(genes);

        // Operator[] access
        CHECK_THAT(chromo[0], Catch::Matchers::WithinRel(3.0f));
        CHECK_THAT(chromo[1], Catch::Matchers::WithinRel(1.0f));
        CHECK_THAT(chromo[2], Catch::Matchers::WithinRel(2.0f));

        // at() with error handling
        auto result0 = chromo.at(0);
        auto result1 = chromo.at(1);
        auto result2 = chromo.at(2);
        auto result_out_of_bounds = chromo.at(3);

        REQUIRE(result0.has_value());
        REQUIRE(result1.has_value());
        REQUIRE(result2.has_value());
        CHECK_FALSE(result_out_of_bounds.has_value());
        CHECK_THAT(*result0, Catch::Matchers::WithinRel(3.0f));
        CHECK_THAT(*result1, Catch::Matchers::WithinRel(1.0f));
        CHECK_THAT(*result2, Catch::Matchers::WithinRel(2.0f));
        CHECK(result_out_of_bounds.error() == chromosome_error::k_index_out_of_bounds);
    }

    SECTION("comparison") {
        std::vector<float> genes1 = {3.0f, 1.0f, 2.0f};
        std::vector<float> genes2 = {3.0f, 1.0f, 2.0f};
        std::vector<float> genes3 = {3.1f, 1.0f, 2.0f};

        chromosome chromo1(genes1);
        chromosome chromo2(genes2);
        chromosome chromo3(genes3);  // Compare contents instead of using operator==
        bool equal = std::equal(chromo1.begin(), chromo1.end(), chromo2.begin(), chromo2.end());
        CHECK(equal);

        bool not_equal =
            !std::equal(chromo1.begin(), chromo1.end(), chromo3.begin(), chromo3.end());
        CHECK(not_equal);  // Test approximate equality
        chromosome chromo4(std::vector<float>{3.0f, 1.0f, 2.0f});
        chromosome chromo5(std::vector<float>{3.0000001f, 1.0f, 2.0f});

        // Compare with epsilon
        bool approx_equal = true;
        for (size_t i = 0; i < chromo4.size(); ++i) {
            if (std::abs(chromo4[i] - chromo5[i]) > 0.0001f) {
                approx_equal = false;
                break;
            }
        }
        CHECK(approx_equal);  // Should be equal within epsilon
    }
    SECTION("genes view") {
        std::vector<float> genes = {3.0f, 1.0f, 2.0f};
        chromosome chromo(genes);

        auto view = chromo.genes();
        REQUIRE(view.size() == 3);
        CHECK_THAT(view[0], Catch::Matchers::WithinRel(3.0F));
        CHECK_THAT(view[1], Catch::Matchers::WithinRel(1.0F));
        CHECK_THAT(view[2], Catch::Matchers::WithinRel(2.0F));

        // Test const view
        const auto& const_chromo = chromo;
        auto const_view = const_chromo.genes();
        REQUIRE(const_view.size() == 3);
        CHECK_THAT(const_view[0], Catch::Matchers::WithinRel(3.0F));
    }
    SECTION("genes access") {
        chromosome chromo(std::vector<float>{3.0F, 1.0F, 2.0F});
        const auto& genes_view = chromo.genes();

        REQUIRE(genes_view.size() == 3);
        CHECK_THAT(genes_view[0], Catch::Matchers::WithinRel(3.0F));
        CHECK_THAT(genes_view[1], Catch::Matchers::WithinRel(1.0F));
        CHECK_THAT(genes_view[2], Catch::Matchers::WithinRel(2.0F));
    }
}