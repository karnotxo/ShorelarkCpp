#ifndef CSHORELARK_APPS_SIMULATION_UI_SIMULATION_CONFIG_H
#define CSHORELARK_APPS_SIMULATION_UI_SIMULATION_CONFIG_H

#include <filesystem>
#include <system_error>
#include <tl/expected.hpp>

#include "simulation/config.h"

namespace cshorelark {

/**
 * @brief UI-specific configuration parameters
 */
struct alignas(8) ui_config {
    float simulation_speed = 1.0F;  ///< Simulation speed multiplier
    bool show_vision_cones = true;  ///< Whether to show vision cones
    bool show_stats = true;         ///< Whether to show statistics
    bool show_grid = false;         ///< Whether to show grid
};

/**
 * @brief Combined simulation and UI configuration
 */
class simulation_window_config {
public:
    /**
     * @brief Constructs a default configuration
     */
    simulation_window_config() = default;

    /**
     * @brief Loads configuration from a file
     * @param path Path to the configuration file
     * @return Expected configuration, with error code if loading failed
     */
    [[nodiscard]] static auto load_from_file(const std::filesystem::path& path)
        -> tl::expected<simulation_window_config, std::error_code>;

    /**
     * @brief Saves the configuration to a file
     * @param path Path where to save the configuration
     * @return Expected path to the saved file with error code if saving failed
     */
    [[nodiscard]] auto save_to_file(const std::filesystem::path& path) const
        -> tl::expected<std::filesystem::path, std::error_code>;

    // Accessors for configuration components
    [[nodiscard]] auto get_simulation() const -> const simulation::config& { return sim_config_; }
    void set_simulation(const simulation::config& config) { sim_config_ = config; }

    [[nodiscard]] auto get_ui() const -> const ui_config& { return ui_config_; }
    void set_ui(const ui_config& config) { ui_config_ = config; }

private:
    simulation::config sim_config_;
    ui_config ui_config_;
};

}  // namespace cshorelark

#endif  // CSHORELARK_APPS_SIMULATION_UI_SIMULATION_CONFIG_H