#include "simulation_config.h"

#include <spdlog/spdlog.h>

#include <filesystem>
#include <fstream>
#include <system_error>
#include <tl/expected.hpp>
#include <toml++/toml.hpp>

namespace cshorelark {

namespace {

// Helper to create a TOML table from WorldConfig
auto create_world_table(const simulation::world_config& config) -> toml::table {
    return toml::table{{"num_foods", static_cast<int64_t>(config.num_foods)},
                       {"num_animals", static_cast<int64_t>(config.num_animals)},
                       {"food_size", config.food_size},
                       {"bird_size", config.bird_size}};
}

// Helper to create a TOML table from BrainEyeConfig
auto create_brain_eye_table(const simulation::brain_eye_config& config) -> toml::table {
    return toml::table{{"fov_range", config.fov_range},
                       {"fov_angle_deg", config.fov_angle_deg},
                       {"num_cells", static_cast<int64_t>(config.num_cells)},
                       {"num_neurons", static_cast<int64_t>(config.num_neurons)}};
}

// Helper to create a TOML table from SimConfig
auto create_sim_table(const simulation::sim_config& config) -> toml::table {
    return toml::table{
        {"speed_min", config.speed_min},
        {"speed_max", config.speed_max},
        {"speed_accel", config.speed_accel},
        {"rotation_accel_deg", config.rotation_accel_deg},
    };
}

// Helper to create a TOML table from GeneticConfig
auto create_genetic_table(const simulation::genetic_config& config) -> toml::table {
    return toml::table{{"mutation_chance", config.mutation_chance},
                       {"mutation_coeff", config.mutation_coeff},
                       {"reverse", config.reverse}};
}

// Helper to create a TOML table from UI config
auto create_ui_table(const ui_config& config) -> toml::table {
    return toml::table{{"simulation_speed", config.simulation_speed},
                       {"show_vision_cones", config.show_vision_cones},
                       {"show_stats", config.show_stats},
                       {"show_grid", config.show_grid}};
}

// Parse world configuration from TOML
auto parse_world_config(const toml::table& table)
    -> tl::expected<simulation::world_config, std::error_code> {
    try {
        simulation::world_config config;

        config.num_foods = static_cast<std::size_t>(table["num_foods"].value_or(60));
        config.num_animals = static_cast<std::size_t>(table["num_animals"].value_or(40));
        config.food_size = table["food_size"].value_or(0.01F);
        config.bird_size = table["bird_size"].value_or(0.015F);

        spdlog::info(
            "Parsed world config: num_foods={}, num_animals={}, food_size={}, bird_size={}",
            config.num_foods, config.num_animals, config.food_size, config.bird_size);

        return config;
    } catch (const std::exception& e) {
        spdlog::error("Failed to parse world config: {}", e.what());
        return tl::unexpected(std::make_error_code(std::errc::invalid_argument));
    }
}

// Parse brain & eye configuration from TOML
auto parse_brain_eye_config(const toml::table& table)
    -> tl::expected<simulation::brain_eye_config, std::error_code> {
    try {
        simulation::brain_eye_config config;

        config.fov_range = table["fov_range"].value_or(0.25F);
        config.fov_angle_deg = table["fov_angle_deg"].value_or(225.0F);
        config.num_cells = static_cast<std::size_t>(table["num_cells"].value_or(9));
        config.num_neurons = static_cast<std::size_t>(table["num_neurons"].value_or(9));

        spdlog::info(
            "Parsed brain & eye config: fov_range={}, fov_angle_deg={}, num_cells={}, "
            "num_neurons={}",
            config.fov_range, config.fov_angle_deg, config.num_cells, config.num_neurons);

        return config;
    } catch (const std::exception& e) {
        spdlog::error("Failed to parse eye config: {}", e.what());
        return tl::unexpected(std::make_error_code(std::errc::invalid_argument));
    }
}

// Parse sim configuration from TOML
auto parse_sim_config(const toml::table& table)
    -> tl::expected<simulation::sim_config, std::error_code> {
    try {
        simulation::sim_config config;

        // Get the pi_over2 constant for default rotation acceleration
        const auto pi_over2 = simulation::constants::k_pi_over2;

        config.speed_min = table["speed_min"].value_or(0.001F);
        config.speed_max = table["speed_max"].value_or(0.005F);
        config.speed_accel = table["speed_accel"].value_or(0.2F);
        config.rotation_accel_deg = table["rotation_accel_deg"].value_or(90.0F);
        config.generation_length = table["generation_length"].value_or(2500);

        spdlog::info(
            "Parsed sim config: speed_min={}, speed_max={}, speed_accel={}, "
            "rotation_accel_deg={}, generation_length={}",
            config.speed_min, config.speed_max, config.speed_accel, config.rotation_accel_deg,
            config.generation_length);

        return config;
    } catch (const std::exception& e) {
        spdlog::error("Failed to parse animal config: {}", e.what());
        return tl::unexpected(std::make_error_code(std::errc::invalid_argument));
    }
}

// Parse genetic configuration from TOML
auto parse_genetic_config(const toml::table& table)
    -> tl::expected<simulation::genetic_config, std::error_code> {
    try {
        simulation::genetic_config config;

        config.mutation_chance = table["mutation_chance"].value_or(0.01F);
        config.mutation_coeff = table["mutation_coeff"].value_or(0.3F);
        config.reverse = static_cast<bool>(table["reverse"].value_or(false));

        spdlog::info("Parsed genetic config: mutation_chance={}, mutation_coeff={}, reverse={}",
                     config.mutation_chance, config.mutation_coeff, config.reverse);

        return config;
    } catch (const std::exception& e) {
        spdlog::error("Failed to parse genetic config: {}", e.what());
        return tl::unexpected(std::make_error_code(std::errc::invalid_argument));
    }
}

// Parse UI configuration from TOML
auto parse_ui_config(const toml::table& table) -> tl::expected<ui_config, std::error_code> {
    try {
        ui_config config;

        config.simulation_speed = table["simulation_speed"].value_or(1.0F);
        config.show_vision_cones = table["show_vision_cones"].value_or(true);
        config.show_stats = table["show_stats"].value_or(true);
        config.show_grid = table["show_grid"].value_or(false);

        return config;
    } catch (const std::exception& e) {
        spdlog::error("Failed to parse UI config: {}", e.what());
        return tl::unexpected(std::make_error_code(std::errc::invalid_argument));
    }
}

}  // namespace

auto simulation_window_config::load_from_file(const std::filesystem::path& path)
    -> tl::expected<simulation_window_config, std::error_code> {
    try {
        auto config = toml::parse_file(path.string());
        simulation_window_config result;

        // Parse simulation config components
        simulation::config sim_config;

        if (const auto* world_table = config["world"].as_table()) {
            auto world_result = parse_world_config(*world_table);
            if (world_result) {
                sim_config.world = *world_result;
            } else {
                return tl::unexpected(world_result.error());
            }
        }

        if (const auto* sim_table = config["sim"].as_table()) {
            auto sim_result = parse_sim_config(*sim_table);
            if (sim_result) {
                sim_config.sim = *sim_result;
            } else {
                return tl::unexpected(sim_result.error());
            }
        }

        if (const auto* genetic_table = config["genetic"].as_table()) {
            auto genetic_result = parse_genetic_config(*genetic_table);
            if (genetic_result) {
                sim_config.genetic = *genetic_result;
            } else {
                return tl::unexpected(genetic_result.error());
            }
        }

        // Parse nested eye config if present
        if (const auto* brain_eye_table = config["brain_eye"].as_table()) {
            auto brain_eye_result = parse_brain_eye_config(*brain_eye_table);
            if (brain_eye_result) {
                sim_config.brain_eye = *brain_eye_result;
            } else {
                return tl::unexpected(brain_eye_result.error());
            }
        }

        result.set_simulation(sim_config);

        // Parse UI config
        if (auto* ui_table = config["ui"].as_table()) {
            auto ui_result = parse_ui_config(*ui_table);
            if (ui_result) {
                result.set_ui(*ui_result);
            } else {
                return tl::unexpected(ui_result.error());
            }
        }

        return result;
    } catch (const toml::parse_error& e) {
        spdlog::error("Failed to parse config file: {}", e.what());
        return tl::unexpected(std::make_error_code(std::errc::invalid_argument));
    } catch (const std::exception& e) {
        spdlog::error("Failed to load config file: {}", e.what());
        return tl::unexpected(std::make_error_code(std::errc::io_error));
    }
}

auto simulation_window_config::save_to_file(const std::filesystem::path& path) const
    -> tl::expected<std::filesystem::path, std::error_code> {
    try {
        toml::table config;
        const auto& sim_config = get_simulation();

        // Add simulation config components
        config.insert("world", create_world_table(sim_config.world));
        config.insert("sim", create_sim_table(sim_config.sim));
        config.insert("genetic", create_genetic_table(sim_config.genetic));
        config.insert("brain_eye", create_brain_eye_table(sim_config.brain_eye));

        // Add UI config
        config.insert("ui", create_ui_table(get_ui()));

        std::ofstream file(path);
        if (!file) {
            return tl::unexpected(std::make_error_code(std::errc::io_error));
        }

        file << config;

        return path;
    } catch (const std::exception& e) {
        spdlog::error("Failed to save config file: {}", e.what());
        return tl::unexpected(std::make_error_code(std::errc::io_error));
    }
}

}  // namespace cshorelark