#ifndef CSHORELARK_GENETIC_ALGORITHM_CHROMOSOME_H
#define CSHORELARK_GENETIC_ALGORITHM_CHROMOSOME_H

/**
 * @file chromosome.h
 * @brief Chromosome class for genetic algorithm implementation
 */

// C++ system headers
#include <random>
#include <string>
#include <vector>

// External library headers
#include <nonstd/span.hpp>  // For std::span
#include <range/v3/range/concepts.hpp>
#include <range/v3/view/interface.hpp>
#include <tl/expected.hpp>

namespace cshorelark::genetic {

/**
 * @brief Error types that can occur during chromosome operations
 */
enum class chromosome_error {
    k_index_out_of_bounds,  ///< Attempted access beyond chromosome bounds
    k_invalid_size,         ///< Invalid chromosome size specified
    k_invalid_gene          ///< Invalid gene value encountered
};

/**
 * @brief Represents genetic information in the genetic algorithm
 *
 * The chromosome class provides a safe, efficient interface for managing
 * genetic information. It uses continuous storage for optimal performance
 * and provides both safe and unchecked access methods.
 */
class chromosome {
public:
    // STL-compatible type aliases
    using value_type = float;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using reference = value_type&;
    using const_reference = const value_type&;
    using pointer = value_type*;
    using const_pointer = const value_type*;
    using iterator = std::vector<value_type>::iterator;
    using const_iterator = std::vector<value_type>::const_iterator;
    using reverse_iterator = std::vector<value_type>::reverse_iterator;
    using const_reverse_iterator = std::vector<value_type>::const_reverse_iterator;
    // Additional type aliases for genetic algorithm specific use
    using span_type = nonstd::span<value_type>;
    using const_span_type = nonstd::span<const value_type>;

    /**
     * @brief Constructs a chromosome from a vector of genes
     * @param genes Vector of gene values to initialize with
     */
    explicit chromosome(std::vector<value_type> genes) noexcept;

    // Prevent copying to match Rust's move semantics
    chromosome(const chromosome&) = delete;
    chromosome& operator=(const chromosome&) = delete;

    // Allow moving
    chromosome(chromosome&&) noexcept = default;
    chromosome& operator=(chromosome&&) noexcept = default;

    /**
     * @brief Safe access to genes with bounds checking
     * @param pos Position to access
     * @return Reference to gene or error if out of bounds
     */
    tl::expected<value_type, chromosome_error> at(size_type pos);
    [[nodiscard]] tl::expected<value_type, chromosome_error> at(size_type pos) const;

    /**
     * @brief Unchecked access to genes - use only when bounds are guaranteed
     * @param pos Position to access
     * @return Reference to gene at position
     */
    reference operator[](size_type pos) noexcept { return genes_[pos]; }
    const_reference operator[](size_type pos) const noexcept { return genes_[pos]; }

    // STL-compatible iterator interface
    iterator begin() noexcept { return genes_.begin(); }
    [[nodiscard]] const_iterator begin() const noexcept { return genes_.begin(); }
    [[nodiscard]] const_iterator cbegin() const noexcept { return genes_.cbegin(); }
    iterator end() noexcept { return genes_.end(); }
    [[nodiscard]] const_iterator end() const noexcept { return genes_.end(); }
    [[nodiscard]] const_iterator cend() const noexcept { return genes_.cend(); }

    // Reverse iterators
    reverse_iterator rbegin() noexcept { return genes_.rbegin(); }
    [[nodiscard]] const_reverse_iterator rbegin() const noexcept { return genes_.rbegin(); }
    [[nodiscard]] const_reverse_iterator crbegin() const noexcept { return genes_.crbegin(); }
    reverse_iterator rend() noexcept { return genes_.rend(); }
    [[nodiscard]] const_reverse_iterator rend() const noexcept { return genes_.rend(); }
    [[nodiscard]] const_reverse_iterator crend() const noexcept { return genes_.crend(); }

    // Capacity
    [[nodiscard]] size_type size() const noexcept { return genes_.size(); }
    [[nodiscard]] bool empty() const noexcept { return genes_.empty(); }

    /**
     * @brief Gets a span view of the genes
     * @return Span containing all genes
     */
    [[nodiscard]] const_span_type genes() const noexcept { return genes_; }

    /**
     * @brief Creates a deep copy of the chromosome
     * @return New chromosome with same genes
     */
    [[nodiscard]] auto clone() const -> chromosome;

    /**
     * @brief Creates a random chromosome
     * @param size Number of genes in the chromosome
     * @param rng Random number generator
     * @param min_value Minimum value for genes
     * @param max_value Maximum value for genes
     * @return A new chromosome with random genes or an error
     */
    [[nodiscard]] static auto random(size_type size, std::mt19937& rng, value_type min_value,
                                     value_type max_value)
        -> tl::expected<chromosome, chromosome_error>;

    /**
     * @brief Get a slice of the chromosome
     * @param start Starting position
     * @param count Number of genes to include
     * @return Span view of the slice or error if invalid range
     */
    [[nodiscard]] auto slice(size_type start,
                             size_type count) -> tl::expected<span_type, chromosome_error>;
    [[nodiscard]] auto slice(size_type start, size_type count) const
        -> tl::expected<const_span_type, chromosome_error>;

    /**
     * @brief Creates a chromosome from a range of values
     * @param first Iterator to the start of the range
     * @param last Iterator to the end of the range
     * @return A new chromosome containing the values from the range
     */
    template <typename InputIt>
    static auto from_range(InputIt first, InputIt last) -> chromosome {
        return chromosome(std::vector<value_type>(first, last));
    }

private:
    std::vector<value_type> genes_;  // Storage for gene values
};

}  // namespace cshorelark::genetic
// Enable range-v3 view support for span
namespace ranges {
template <typename T>
inline constexpr bool enable_view<nonstd::span<T>> = true;
}  // namespace ranges

#endif  // CSHORELARK_GENETIC_ALGORITHM_CHROMOSOME_H_