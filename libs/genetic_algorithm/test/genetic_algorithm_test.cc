#include "genetic_algorithm/genetic_algorithm.h"

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <memory>
#include <utility>
#include <vector>

#include "genetic_algorithm/individual.h"
#include "test_individual.h"

using Catch::Matchers::WithinRel;

namespace cshorelark::genetic {
namespace {

// Helper function to create a test population
std::vector<std::unique_ptr<individual>> create_test_population(std::size_t size,
                                                                std::size_t num_genes = 3) {
    std::vector<std::unique_ptr<individual>> population;
    population.reserve(size);

    for (std::size_t i = 0; i < size; ++i) {
        std::vector<float> genes(num_genes, static_cast<float>(i));
        auto ind = std::make_unique<test_individual>(chromosome(genes));
        ind->set_fitness(static_cast<float>(i));
        population.push_back(std::move(ind));
    }

    return population;
}

}  // namespace
}  // namespace cshorelark::genetic