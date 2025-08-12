#ifndef CSHORELARK_SIMULATION_STATISTICS_H
#define CSHORELARK_SIMULATION_STATISTICS_H

#include <fmt/core.h>
#include <fmt/format.h>  // Third-party headers

#include <cstddef>
#include <string>  // C++ headers

#include "genetic_algorithm/statistics.h"  // Project headers

namespace cshorelark::simulation {

/**
 * @brief Statistics about a simulation generation
 *
 * Wraps genetic algorithm statistics with additional simulation-specific
 * information like the generation number.
 */
class statistics {
public:
    /**
     * @brief Construct simulation statistics
     *
     * @param generation The generation number (0-based)
     * @param ga_stats The genetic algorithm statistics for this generation
     */
    statistics(size_t generation, genetic::statistics ga_stats)
        : generation_(generation), ga_stats_(ga_stats) {}

    // Accessors
    [[nodiscard]] auto generation() const noexcept -> size_t { return generation_; }
    [[nodiscard]] auto ga_stats() const noexcept -> const genetic::statistics& { return ga_stats_; }

    /**
     * @brief Format statistics as a human-readable string
     */
    [[nodiscard]] auto to_string() const -> std::string {
        return fmt::format(
            "Generation {}:\n"
            "  Min: {:.2F}\n"
            "  Max: {:.2F}\n"
            "  Avg: {:.2F}\n"
            "  Median: {:.2F}",
            generation_, ga_stats_.min_fitness(), ga_stats_.max_fitness(), ga_stats_.avg_fitness(),
            ga_stats_.median_fitness());
    }

private:
    size_t generation_;
    genetic::statistics ga_stats_;
};

}  // namespace cshorelark::simulation

// fmt formatter specialization
template <>
struct fmt::formatter<cshorelark::simulation::statistics> {
    static constexpr auto parse(format_parse_context& ctx) -> decltype(ctx.begin()) {
        return ctx.begin();
    }

    template <typename FormatContext>
    auto format(const cshorelark::simulation::statistics& stats,
                FormatContext& ctx) const -> decltype(ctx.out()) {
        return fmt::format_to(ctx.out(), "{}", stats.to_string());
    }
};

#endif  // CSHORELARK_SIMULATION_STATISTICS_H_