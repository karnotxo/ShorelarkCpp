#include "genetic_algorithm/chromosome.h"

#include <random>
#include <tl/expected.hpp>
#include <utility>
#include <vector>

namespace cshorelark::genetic {

chromosome::chromosome(std::vector<value_type> genes) noexcept : genes_(std::move(genes)) {}

tl::expected<chromosome, chromosome_error> chromosome::random(size_type size, std::mt19937& rng,
                                                              value_type min_value,
                                                              value_type max_value) {
    if (size == 0) {
        return tl::unexpected(chromosome_error::k_invalid_size);
    }
    if (min_value > max_value) {
        return tl::unexpected(chromosome_error::k_invalid_gene);
    }

    std::vector<value_type> genes;
    genes.reserve(size);
    std::uniform_real_distribution<value_type> dist(min_value, max_value);

    for (size_type i = 0; i < size; ++i) {
        genes.push_back(dist(rng));
    }

    return chromosome(std::move(genes));
}

tl::expected<chromosome::value_type, chromosome_error> chromosome::at(size_type pos) {
    if (pos >= genes_.size()) {
        return tl::unexpected(chromosome_error::k_index_out_of_bounds);
    }
    return genes_[pos];
}

tl::expected<chromosome::value_type, chromosome_error> chromosome::at(size_type pos) const {
    if (pos >= genes_.size()) {
        return tl::unexpected(chromosome_error::k_index_out_of_bounds);
    }
    return genes_[pos];
}

tl::expected<chromosome::span_type, chromosome_error> chromosome::slice(size_type start,
                                                                        size_type count) {
    if (start >= genes_.size() || start + count > genes_.size()) {
        return tl::unexpected(chromosome_error::k_index_out_of_bounds);
    }
    return nonstd::span<value_type>(genes_.data() + start, count);
}

tl::expected<chromosome::const_span_type, chromosome_error> chromosome::slice(
    size_type start, size_type count) const {
    if (start >= genes_.size() || start + count > genes_.size()) {
        return tl::unexpected(chromosome_error::k_index_out_of_bounds);
    }
    return nonstd::span<const value_type>(genes_.data() + start, count);
}

chromosome chromosome::clone() const { return chromosome(std::vector<value_type>(genes_)); }

}  // namespace cshorelark::genetic