#include "genetic_algorithm/crossover.h"

#include <cstddef>
#include <tl/expected.hpp>
#include <utility>
#include <vector>

#include "genetic_algorithm/chromosome.h"
#include "genetic_algorithm/genetic_error.h"

namespace cshorelark::genetic {

auto single_point_crossover::crossover(const chromosome& parent1, const chromosome& parent2,
                                       cshorelark::random::random_generator& random) const
    -> tl::expected<chromosome, genetic_error> {
    if (parent1.size() != parent2.size()) {
        return tl::unexpected(genetic_error{genetic_error_code::k_invalid_parent_size,
                                            "Parent chromosomes must have the same size"});
    }

    std::vector<float> child_genes;
    child_genes.reserve(parent1.size());

    // Choose a random crossover point
    std::size_t crossover_point =
        static_cast<std::size_t>(random.generate_position() * parent1.size());

    // Take genes from parent1 up to the crossover point
    for (std::size_t i = 0; i < crossover_point; ++i) {
        child_genes.push_back(parent1[i]);
    }

    // Take genes from parent2 after the crossover point
    for (std::size_t i = crossover_point; i < parent2.size(); ++i) {
        child_genes.push_back(parent2[i]);
    }

    return chromosome(std::move(child_genes));
}

auto uniform_crossover::crossover(const chromosome& parent1, const chromosome& parent2,
                                  cshorelark::random::random_generator& random) const
    -> tl::expected<chromosome, genetic_error> {
    if (parent1.size() != parent2.size()) {
        return tl::unexpected(genetic_error{genetic_error_code::k_invalid_parent_size,
                                            "Parent chromosomes must have the same size"});
    }

    std::vector<float> child_genes;
    child_genes.reserve(parent1.size());

    // Match Rust implementation's simpler gene selection
    for (std::size_t i = 0; i < parent1.size(); ++i) {
        child_genes.push_back((random.generate_weight() < swap_probability_) ? parent1[i]
                                                                             : parent2[i]);
    }

    return chromosome(std::move(child_genes));
}

}  // namespace cshorelark::genetic