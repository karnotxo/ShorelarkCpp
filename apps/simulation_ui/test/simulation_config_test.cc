#include "../src/simulation_config.h"

#include <catch2/catch_test_macros.hpp>
#include <filesystem>
#include <fstream>

namespace {

// Test constants to avoid magic numbers
constexpr float k_test_speed_min = 0.002F;
constexpr float k_test_speed_max = 0.01F;
constexpr float k_test_speed_accel = 0.3F;
constexpr float k_test_rotation_accel_deg = 90.0F;
constexpr float k_test_food_size = 0.01F;
constexpr float k_test_bird_size = 0.015F;
constexpr std::size_t k_test_num_foods = 200;
constexpr std::size_t k_test_num_animals = 100;
constexpr float k_test_fov_range = 0.5F;
constexpr float k_test_fov_angle_deg = 180.0F;
constexpr std::size_t k_test_num_cells = 12;
constexpr std::size_t k_test_num_neurons = 15;
constexpr float k_test_mutation_chance = 0.02F;
constexpr float k_test_mutation_coeff = 0.4F;
constexpr float k_test_simulation_speed = 2.0F;
constexpr std::size_t k_test_generation_length = 2500;

}  // namespace

TEST_CASE("SimulationConfig can save and load configuration", "[config]") {
    // Create a temporary file for testing
    auto temp_path = std::filesystem::temp_directory_path() / "test_config.toml";

    SECTION("Default configuration can be saved and loaded") {
        const cshorelark::simulation_window_config original;

        // Save the configuration
        auto save_result = original.save_to_file(temp_path);
        REQUIRE(save_result.has_value());

        // Load the configuration
        auto load_result = cshorelark::simulation_window_config::load_from_file(temp_path);
        REQUIRE(load_result.has_value());

        const auto& loaded = load_result.value();

        // Verify world config
        REQUIRE(loaded.get_simulation().world.num_foods ==
                original.get_simulation().world.num_foods);
        REQUIRE(loaded.get_simulation().world.food_size ==
                original.get_simulation().world.food_size);
        REQUIRE(loaded.get_simulation().world.num_animals ==
                original.get_simulation().world.num_animals);
        REQUIRE(loaded.get_simulation().world.bird_size ==
                original.get_simulation().world.bird_size);

        // Verify sim config
        const auto& loaded_sim = loaded.get_simulation().sim;
        const auto& original_sim = original.get_simulation().sim;
        REQUIRE(loaded_sim.speed_min == original_sim.speed_min);
        REQUIRE(loaded_sim.speed_max == original_sim.speed_max);
        REQUIRE(loaded_sim.speed_accel == original_sim.speed_accel);
        REQUIRE(loaded_sim.rotation_accel_deg == original_sim.rotation_accel_deg);
        REQUIRE(loaded_sim.generation_length == original_sim.generation_length);

        // Verify brain eye config
        const auto& loaded_eye = loaded.get_simulation().brain_eye;
        const auto& original_eye = original.get_simulation().brain_eye;
        REQUIRE(loaded_eye.fov_range == original_eye.fov_range);
        REQUIRE(loaded_eye.fov_angle_deg == original_eye.fov_angle_deg);
        REQUIRE(loaded_eye.num_cells == original_eye.num_cells);
        REQUIRE(loaded_eye.num_neurons == original_eye.num_neurons);

        // Verify genetic config
        const auto& loaded_genetic = loaded.get_simulation().genetic;
        const auto& original_genetic = original.get_simulation().genetic;
        REQUIRE(loaded_genetic.mutation_chance == original_genetic.mutation_chance);
        REQUIRE(loaded_genetic.mutation_coeff == original_genetic.mutation_coeff);
        REQUIRE(loaded_genetic.reverse == original_genetic.reverse);

        // Verify UI config
        REQUIRE(loaded.get_ui().simulation_speed == original.get_ui().simulation_speed);
        REQUIRE(loaded.get_ui().show_vision_cones == original.get_ui().show_vision_cones);
        REQUIRE(loaded.get_ui().show_stats == original.get_ui().show_stats);
        REQUIRE(loaded.get_ui().show_grid == original.get_ui().show_grid);
    }

    SECTION("Custom configuration can be saved and loaded") {
        cshorelark::simulation_window_config config;

        // Modify simulation config
        auto sim_config = config.get_simulation();

        // Modify world config
        sim_config.world.num_foods = k_test_num_foods;
        sim_config.world.food_size = k_test_food_size;
        sim_config.world.num_animals = k_test_num_animals;
        sim_config.world.bird_size = k_test_bird_size;

        // Modify sim config
        sim_config.sim.speed_min = k_test_speed_min;
        sim_config.sim.speed_max = k_test_speed_max;
        sim_config.sim.speed_accel = k_test_speed_accel;
        sim_config.sim.rotation_accel_deg = k_test_rotation_accel_deg;
        sim_config.sim.generation_length = k_test_generation_length;

        // Modify brain eye config
        sim_config.brain_eye.fov_range = k_test_fov_range;
        sim_config.brain_eye.fov_angle_deg = k_test_fov_angle_deg;
        sim_config.brain_eye.num_cells = k_test_num_cells;
        sim_config.brain_eye.num_neurons = k_test_num_neurons;

        // Modify genetic config
        sim_config.genetic.mutation_chance = k_test_mutation_chance;
        sim_config.genetic.mutation_coeff = k_test_mutation_coeff;
        sim_config.genetic.reverse = false;

        config.set_simulation(sim_config);

        // Modify UI config
        auto ui_config = config.get_ui();
        ui_config.simulation_speed = k_test_simulation_speed;
        ui_config.show_vision_cones = false;
        ui_config.show_stats = false;
        ui_config.show_grid = true;
        config.set_ui(ui_config);

        // Save the configuration
        auto save_result = config.save_to_file(temp_path);
        REQUIRE(save_result.has_value());

        // Load the configuration
        auto load_result = cshorelark::simulation_window_config::load_from_file(temp_path);
        REQUIRE(load_result.has_value());

        const auto& loaded = load_result.value();

        // Verify world config
        REQUIRE(loaded.get_simulation().world.num_foods == sim_config.world.num_foods);
        REQUIRE(loaded.get_simulation().world.food_size == sim_config.world.food_size);
        REQUIRE(loaded.get_simulation().world.num_animals == sim_config.world.num_animals);
        REQUIRE(loaded.get_simulation().world.bird_size == sim_config.world.bird_size);

        // Verify sim config
        const auto& loaded_sim = loaded.get_simulation().sim;
        REQUIRE(loaded_sim.speed_min == sim_config.sim.speed_min);
        REQUIRE(loaded_sim.speed_max == sim_config.sim.speed_max);
        REQUIRE(loaded_sim.speed_accel == sim_config.sim.speed_accel);
        REQUIRE(loaded_sim.rotation_accel_deg == sim_config.sim.rotation_accel_deg);
        REQUIRE(loaded_sim.generation_length == sim_config.sim.generation_length);

        // Verify brain eye config
        const auto& loaded_eye = loaded.get_simulation().brain_eye;
        REQUIRE(loaded_eye.fov_range == sim_config.brain_eye.fov_range);
        REQUIRE(loaded_eye.fov_angle_deg == sim_config.brain_eye.fov_angle_deg);
        REQUIRE(loaded_eye.num_cells == sim_config.brain_eye.num_cells);
        REQUIRE(loaded_eye.num_neurons == sim_config.brain_eye.num_neurons);

        // Verify genetic config
        const auto& loaded_genetic = loaded.get_simulation().genetic;
        REQUIRE(loaded_genetic.mutation_chance == sim_config.genetic.mutation_chance);
        REQUIRE(loaded_genetic.mutation_coeff == sim_config.genetic.mutation_coeff);
        REQUIRE(loaded_genetic.reverse == sim_config.genetic.reverse);

        // Verify UI config
        REQUIRE(loaded.get_ui().simulation_speed == ui_config.simulation_speed);
        REQUIRE(loaded.get_ui().show_vision_cones == ui_config.show_vision_cones);
        REQUIRE(loaded.get_ui().show_stats == ui_config.show_stats);
        REQUIRE(loaded.get_ui().show_grid == ui_config.show_grid);
    }

    SECTION("Loading invalid file returns error") {
        // Try to load non-existent file
        auto load_result = cshorelark::simulation_window_config::load_from_file("non_existent.toml");
        REQUIRE_FALSE(load_result.has_value());

        // Create file with invalid TOML
        {
            std::ofstream out(temp_path);
            out << "invalid = [ toml = content";
        }

        load_result = cshorelark::simulation_window_config::load_from_file(temp_path);
        REQUIRE_FALSE(load_result.has_value());
    }

    // Cleanup
    std::filesystem::remove(temp_path);
}