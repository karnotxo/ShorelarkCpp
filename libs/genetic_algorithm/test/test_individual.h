#ifndef CSHORELARK_GENETIC_ALGORITHM_TEST_INDIVIDUAL_H
#define CSHORELARK_GENETIC_ALGORITHM_TEST_INDIVIDUAL_H

#include <utility>

#include "genetic_algorithm/chromosome.h"
#include "genetic_algorithm/genetic_error.h"
#include "genetic_algorithm/individual.h"

namespace cshorelark::genetic {

using cshorelark::genetic::chromosome;
using cshorelark::genetic::genetic_error;

/**
 * @brief Concrete implementation of individual for testing
 *
 * This class provides a simple implementation of the individual interface
 * to use in tests.
 */
class test_individual : public individual {
public:
    explicit test_individual(chromosome&& chrom)
        : chromosome_(std::move(chrom)) {}

    // Implement Individual interface
    [[nodiscard]] auto get_fitness() const -> float override {
        if (!has_fitness_) {
            throw std::runtime_error("Fitness value not set");
        }
        return fitness_;
    }

    [[nodiscard]] auto get_chromosome() const -> const chromosome& override { return chromosome_; }

    // Test-specific methods
    void set_fitness(float fitness) {
        has_fitness_ = true;
        fitness_ = fitness;
    }
    [[nodiscard]] auto has_fitness() const -> bool { return has_fitness_; }

    [[nodiscard]] auto get_chrom() const -> const chromosome& { return chromosome_; }
    [[nodiscard]] auto clone() const -> std::unique_ptr<test_individual> {
        // Create a copy of the chromosome
        auto clone = std::make_unique<test_individual>(chromosome_.clone());
        if (has_fitness_) {
            clone->set_fitness(fitness_);
        }
        return clone;
    }
    [[nodiscard]] auto create_offspring(cshorelark::genetic::chromosome offspring_chromosome) const
        -> tl::expected<std::unique_ptr<test_individual>, genetic_error> {
        try {
            return std::make_unique<test_individual>(std::move(offspring_chromosome));
        } catch (const std::exception& e) {
            return tl::unexpected(
                genetic_error{genetic_error_code::k_invalid_chromosome,
                              std::string("Failed to create offspring: ") + e.what()});
        }
    }

    // Helper method for creating instances
    [[nodiscard]] static auto create(const std::vector<float>& genes) -> test_individual {
        return test_individual(cshorelark::genetic::chromosome(genes));
    }

    // Static factory method required by genetic_algorithm
    [[nodiscard]] static auto from_chromosome(genetic::chromosome&& chromosome)
        -> tl::expected<std::unique_ptr<individual>, genetic_error> {
        try {
            // Create a test_individual instance wrapped in a unique_ptr to individual base class
            std::unique_ptr<individual> new_individual =
                std::make_unique<test_individual>(std::move(chromosome));
            return new_individual;
        } catch (const std::exception& e) {
            return tl::unexpected(
                genetic_error{genetic_error_code::k_invalid_chromosome,
                              std::string("Failed to create test_individual: ") + e.what()});
        }
    }

private:
    chromosome chromosome_;
    bool has_fitness_ = false;
    float fitness_ = 0.0F;
};

}  // namespace cshorelark::genetic

#endif  // CSHORELARK_GENETIC_ALGORITHM_TEST_INDIVIDUAL_H
