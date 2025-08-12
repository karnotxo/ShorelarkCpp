#include "genetic_algorithm/mutation.h"

namespace cshorelark::genetic {

gaussian_mutation::gaussian_mutation(float mutation_probability, float mutation_strength) noexcept
    : mutation_probability_(mutation_probability), mutation_strength_(mutation_strength) {}

tl::expected<void, genetic_error> gaussian_mutation::mutate(
    chromosome& child, cshorelark::random::random_generator& random) const {
    // Rust implementation
    /*for gene in child.iter_mut() {
        let sign = if rng.gen_bool(0.5) { -1.0 } else { 1.0 };

        if rng.gen_bool(self.chance as _) {
            *gene += sign * self.coeff * rng.gen::<f32>();
        }
    }*/

    // Mutate each gene with configured probability
    for (auto& gene : child) {
        if (random.generate_position() < mutation_probability_) {
            // Generate mutation amount from normal distribution
            float mutation = random.generate_position() * mutation_strength_;
            // Apply random sign
            mutation = random.generate_weight() > 0 ? mutation : -mutation;
            // Apply mutation to gene
            gene += mutation;
        }
    }

    return {};
}

uniform_mutation::uniform_mutation(float mutation_probability, float min_value,
                                   float max_value) noexcept
    : mutation_probability_(mutation_probability), min_value_(min_value), max_value_(max_value) {}

tl::expected<void, genetic_error> uniform_mutation::mutate(
    chromosome& child, cshorelark::random::random_generator& random_gen) const {
    // Iterate through each gene in the chromosome
    for (auto& gene : child) {
        // Check if mutation should occur based on probability
        if (random_gen.generate_position() < mutation_probability_) {
            // Generate a new random value uniformly within the specified range
            gene = min_value_ + random_gen.generate_position() * (max_value_ - min_value_);
        }
    }
    return {};  // Indicate success
}

}  // namespace cshorelark::genetic