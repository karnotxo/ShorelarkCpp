#include "simulation_window.h"

#include <imgui.h>
#include <imgui_internal.h>
#include <spdlog/spdlog.h>
#include <stdint.h>  // For uintptr_t

#include <algorithm>
#include <chrono>
#include <cmath>    // For cos() and sin()
#include <cstring>  // For strncpy
#include <filesystem>
#include <mutex>
#include <sstream>
#include <thread>

#include "ImFileDialog.h"
#include "imgui_stdlib.h"
#include "simulation/animal.h"
#include "simulation/constants.h"
#include "simulation/simulation.h"
#include "stb_image.h"

namespace cshorelark {

namespace {

auto rotate_vector(const ImVec2 &vector_in, float angle) -> ImVec2 {
    const float cos_angle = std::cosf(angle);
    const float sin_angle = std::sinf(angle);
    return ImVec2{vector_in.x * cos_angle - vector_in.y * sin_angle,
                  vector_in.x * sin_angle + vector_in.y * cos_angle};
}

constexpr auto min_sleep_time_ms = 10;  // Minimum sleep time in milliseconds

}  // namespace

simulation_window::simulation_window()
    : paused_(false),
      evolution_in_progress_(false),
      thread_should_exit_(false),
      gui_data_updated_(false),
      last_step_time_(std::chrono::steady_clock::now()),
      elapsed_time_(0.0f),
      step_interval_(0.016f) {  // Default step interval is 16ms (60 FPS)
    spdlog::debug("Creating simulation window");

    // Reset ImGui INI settings to clear any saved window positions/states
    ImGui::GetIO().IniFilename = NULL;  // Disable saving/loading window positions altogether

    // Initialize default configuration path
    default_config_path_ = std::filesystem::current_path() / "cfg";
    spdlog::debug("Default config path set to: {}", default_config_path_.string());

    // Create cfg directory if it doesn't exist
    if (!std::filesystem::exists(default_config_path_)) {
        std::filesystem::create_directory(default_config_path_);
        spdlog::info("Created configuration directory: {}", default_config_path_.string());
    }

    reset_world();

    // Initialize console with welcome message
    console_input_buffer_.reserve(256);
    console_history_.push_back("Welcome to Shorelark C++ Simulation");
    console_history_.push_back("Type 'help' for a list of commands");

    // Start the simulation thread
    start_simulation_thread();

    spdlog::info("Simulation window created successfully");
}

simulation_window::~simulation_window() {
    spdlog::debug("Destroying simulation window");
    stop_simulation_thread();
    spdlog::debug("Simulation window destroyed");
}

void simulation_window::reset_world() {
    spdlog::info("Resetting simulation world");
    const auto &sim_config = config_.get_simulation();

    // Create new world with configured dimensions
    simulation_ = std::make_unique<simulation::simulation>(
        std::move(simulation::simulation::random(sim_config, random_)));

    elapsed_time_ = 0.0F;
    spdlog::info("World reset complete");
}

void simulation_window::spawn_animal() { simulation_->spawn_animal(random_); }

void simulation_window::spawn_food() { simulation_->spawn_food(random_); }

void simulation_window::render() {
    spdlog::trace("Rendering simulation window");

    // Access the GUI data safely from the simulation thread
    {
        std::lock_guard<std::mutex> lock(gui_data_mutex_);
        // Reset the updated flag
        gui_data_updated_ = false;
    }

    // Get full viewport size
    const ImGuiViewport *viewport = ImGui::GetMainViewport();

    // Create the main simulation window that fills the viewport
    const ImGuiWindowFlags sim_world_flags =
        ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse |
        ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoBackground;

    // Position and size the simulation window to fill the viewport
    ImGui::SetNextWindowPos(viewport->Pos);
    ImGui::SetNextWindowSize(viewport->Size);
    ImGui::Begin("Simulation World", nullptr, sim_world_flags);
    render_world();
    ImGui::End();

    // Set up flags for floating windows
    ImGuiWindowFlags floating_flags = ImGuiWindowFlags_AlwaysAutoResize |
                                      ImGuiWindowFlags_NoFocusOnAppearing |
                                      ImGuiWindowFlags_NoSavedSettings;

    // Create a separate set of flags for the console window that allows resizing
    ImGuiWindowFlags console_flags =
        ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoSavedSettings;

    // Helper to check if initial window positioning is needed
    static auto need_initial_positioning = [](const char *name) -> bool {
        ImGuiWindow *window = ImGui::FindWindowByName(name);
        return !window || window->Collapsed;
    };

    // Controls window - floating at top-left corner
    ImGui::SetNextWindowBgAlpha(0.85F);
    if (need_initial_positioning("Controls")) {
        ImGui::SetNextWindowPos(ImVec2(viewport->WorkPos.x + 10, viewport->WorkPos.y + 10),
                                ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(ImVec2(400, 200), ImGuiCond_FirstUseEver);
    }
    ImGui::Begin("Controls", nullptr, floating_flags);
    render_controls();
    ImGui::End();

    // Statistics window - floating at top-right corner
    ImGui::SetNextWindowBgAlpha(0.85F);
    if (need_initial_positioning("Statistics")) {
        ImGui::SetNextWindowPos(
            ImVec2(viewport->WorkPos.x + viewport->WorkSize.x - 310, viewport->WorkPos.y + 10),
            ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(ImVec2(300, 200), ImGuiCond_FirstUseEver);
    }
    ImGui::Begin("Statistics", nullptr, floating_flags);
    render_statistics();
    ImGui::End();

    // Config window - floating at bottom-left
    ImGui::SetNextWindowBgAlpha(0.85F);
    if (need_initial_positioning("Configuration")) {
        ImGui::SetNextWindowPos(
            ImVec2(viewport->WorkPos.x + 10, viewport->WorkPos.y + viewport->WorkSize.y - 310),
            ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(ImVec2(400, 300), ImGuiCond_FirstUseEver);
    }
    ImGui::Begin("Configuration", nullptr, floating_flags);
    render_config_controls();
    ImGui::End();

    // Command console window - floating at bottom-right
    ImGui::SetNextWindowBgAlpha(0.85F);
    if (need_initial_positioning("Command Console")) {
        ImGui::SetNextWindowPos(ImVec2(viewport->WorkPos.x + viewport->WorkSize.x - 570,
                                       viewport->WorkPos.y + viewport->WorkSize.y - 410),
                                ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(ImVec2(560, 400), ImGuiCond_FirstUseEver);
    }
    ImGui::Begin("Command Console", nullptr, console_flags);
    render_console();
    ImGui::End();

    // Process file dialog if it's open
    if (file_dialog_open_ && !ifd::FileDialog::Instance().IsDone("ConfigPathDlg")) {
        ifd::FileDialog::Instance().IsDone(
            "ConfigPathDlg");  // This call actually renders the dialog
    }

    spdlog::trace("Finished rendering simulation window");
}

void simulation_window::render_world() {
    spdlog::trace("Rendering world");
    const auto &world_config = config_.get_simulation().world;
    const auto &ui_config = config_.get_ui();

    // Calculate window size while maintaining aspect ratio
    ImVec2 window_size = ImGui::GetContentRegionAvail();

    // Ensure minimum size to prevent assertion errors
    window_size.x = std::max(window_size.x, 1.0F);
    window_size.y = std::max(window_size.y, 1.0F);

    if (window_size.x / window_size.y > 1.0F) {
        window_size.x = window_size.y;
    } else {
        window_size.y = window_size.x;
    }

    // Center the canvas in the available space
    ImVec2 window_pos = ImGui::GetCursorScreenPos();
    const float offset_x = (ImGui::GetContentRegionAvail().x - window_size.x) / 2;
    if (offset_x > 0)
        window_pos.x += offset_x;

    const ImVec2 canvas_pos = window_pos;
    const ImVec2 canvas_size = window_size;

    // Ensure canvas has non-zero size
    if (canvas_size.x <= 0.0F || canvas_size.y <= 0.0F) {
        spdlog::warn("Invalid canvas size: {}x{}", canvas_size.x, canvas_size.y);
        return;
    }

    ImDrawList *draw_list = ImGui::GetWindowDrawList();

    // Add background with a darker color for better contrast
    draw_list->AddRectFilled(canvas_pos,
                             ImVec2(canvas_pos.x + canvas_size.x, canvas_pos.y + canvas_size.y),
                             IM_COL32(30, 30, 30, 255));

    // Add a border around the simulation area
    draw_list->AddRect(canvas_pos,
                       ImVec2(canvas_pos.x + canvas_size.x, canvas_pos.y + canvas_size.y),
                       IM_COL32(100, 100, 100, 255));

    // Calculate scale factors - now scaling from [0,1] to screen coordinates
    const float scale = std::min(canvas_size.x, canvas_size.y);

    // Scale the radius from normalized units to screen pixels
    const float min_radius = 2.0F;  // Minimum radius in pixels
    const auto food_size = world_config.food_size * scale;
    const float radius = std::max(food_size, min_radius);
    // Draw food with increased size and brighter color
    spdlog::trace("Drawing {} food items", gui_data_.foods.size());
    for (const auto &food : gui_data_.foods) {
        const float pos_x = canvas_pos.x + food.pos_x * scale;
        const float pos_y = canvas_pos.y + food.pos_y * scale;
        // Draw food with brighter green color
        draw_list->AddCircleFilled(ImVec2(pos_x, pos_y), radius, IM_COL32(50, 255, 50, 255));
    }

    // Scale the radius from normalized units to screen pixels
    const float bird_min_radius = 3.0F;  // Minimum radius in pixels
    const auto bird_size = world_config.bird_size * scale;
    const float bird_radius = std::max(bird_size, bird_min_radius);
    const float fov_degrees = config_.get_simulation().brain_eye.fov_angle_deg;
    const float fov_radians = fov_degrees * (simulation::constants::k_pi / 180.0f);
    // Draw birds with increased size
    spdlog::trace("Drawing {} birds", gui_data_.birds.size());
    for (const auto &bird : gui_data_.birds) {
        const float position_x = canvas_pos.x + bird.pos_x * scale;
        const float position_y = canvas_pos.y + bird.pos_y * scale;

        // Draw body with brighter white color
        draw_list->AddCircleFilled(ImVec2(position_x, position_y), bird_radius,
                                   IM_COL32(255, 255, 255, 255));

        // Use bird.rotation directly for correct direction
        const float display_rotation = bird.rotation;

        // Draw direction indicator
        const float direction_length = bird_radius * 2.5f;  // Made longer
        const ImVec2 direction = rotate_vector(ImVec2(direction_length, 0), display_rotation);
        draw_list->AddLine(ImVec2(position_x, position_y),
                           ImVec2(position_x + direction.x, position_y + direction.y),
                           IM_COL32(255, 50, 50, 255), 2.0F);

        // Draw vision cone if enabled with increased visibility
        if (ui_config.show_vision_cones) {
            const float start_angle = display_rotation - fov_radians * 0.5F;
            const float end_angle = display_rotation + fov_radians * 0.5F;
            const float vision_radius = radius * 6.0F;  // Made larger
            draw_list->PathArcTo(ImVec2(position_x, position_y), vision_radius, start_angle,
                                 end_angle, 32);
            draw_list->PathStroke(IM_COL32(255, 255, 0, 128), ImDrawFlags_None, 2.0F);
        }

        // Add invisible button for tooltip interaction
        ImGui::SetCursorScreenPos(ImVec2(position_x - bird_radius, position_y - bird_radius));
        ImGui::InvisibleButton(
            ("##animal" + std::to_string(reinterpret_cast<uintptr_t>(&bird))).c_str(),
            ImVec2(bird_radius * 2, bird_radius * 2));

        // Show tooltip when hovering over the animal
        if (ImGui::IsItemHovered()) {
            ImGui::BeginTooltip();
            ImGui::Text("Animal Statistics:");
            ImGui::Separator();
            ImGui::Text("Fitness: %zu", bird.fitness);
            ImGui::Text("Speed: %.3f", bird.speed);
            ImGui::Text("Orientation: %.2f°", bird.rotation * 180.0f / simulation::constants::k_pi);
            ImGui::Text("Position: (%.1f, %.1f)", bird.pos_x, bird.pos_y);
            ImGui::EndTooltip();
        }
    }

    // Add invisible button for interaction
    ImVec2 safe_canvas_size = ImVec2(std::max(canvas_size.x, 1.0F), std::max(canvas_size.y, 1.0F));
    ImGui::InvisibleButton("canvas", safe_canvas_size,
                           ImGuiButtonFlags_MouseButtonLeft | ImGuiButtonFlags_MouseButtonRight);

    // Handle mouse interactions with the simulation
    if (ImGui::IsItemHovered()) {
        // Implement mouse interaction here if needed
        // Example: spawn food when clicked
        if (ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
            ImVec2 mouse_pos = ImGui::GetMousePos();
            float world_x = (mouse_pos.x - canvas_pos.x) / scale;
            float world_y = (mouse_pos.y - canvas_pos.y) / scale;

            if (world_x >= 0 && world_x < 1.0F && world_y >= 0 && world_y < 1.0F) {
                simulation_->spawn_food(world_x, world_y);
                spdlog::debug("Food spawned via mouse click at ({}, {})", world_x, world_y);

                // Update GUI data after spawning food to show it immediately
                update_data();
            }
        }
    }
}

void simulation_window::render_controls() {
    ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x * 0.5F);

    // Simulation controls
    auto ui = config_.get_ui();
    ImGui::SliderFloat("Speed", &ui.simulation_speed, 0.1F, 10.0F, "%.1fx");
    config_.set_ui(ui);

    if (ImGui::Button(paused_ ? "Resume" : "Pause")) {
        paused_ = !paused_;
    }

    ImGui::SameLine();
    if (ImGui::Button("Reset")) {
        reset_world();
        elapsed_time_ = 0.0F;
        best_fitness_ = 0.0F;
        avg_fitness_ = 0.0F;
    }

    ImGui::SameLine();
    static int train_generations = 1;
    ImGui::PushItemWidth(100);
    ImGui::InputInt("##train_gens", &train_generations);
    ImGui::PopItemWidth();

    ImGui::SameLine();
    if (ImGui::Button("Train") && train_generations > 0) {
        auto result = train(static_cast<size_t>(train_generations));
        console_history_.emplace_back(result);
    }

    // World configuration
    if (ImGui::TreeNode("World Config")) {
        bool config_changed = false;
        auto sim_config = config_.get_simulation();
        auto &world_config = sim_config.world;

        // World size is now fixed at 1.0 (normalized)
        ImGui::TextDisabled("World Size: 1.0 (normalized)");

        config_changed |= ImGui::SliderInt(
            "Food Count", reinterpret_cast<int *>(&world_config.num_foods), 5, 100);
        config_changed |=
            ImGui::SliderFloat("Food Size", &world_config.food_size, 0.001F, 0.05F, "%.3f");
        config_changed |= ImGui::SliderInt(
            "Animal Count", reinterpret_cast<int *>(&world_config.num_animals), 1, 100);
        config_changed |=
            ImGui::SliderFloat("Bird Size", &world_config.bird_size, 0.001F, 0.05F, "%.3f");

        if (config_changed) {
            config_.set_simulation(sim_config);
            reset_world();
        }
        ImGui::TreePop();
    }

    // Simulation configuration
    if (ImGui::TreeNode("Simulation Config")) {
        bool config_changed = false;
        auto sim_config = config_.get_simulation();
        auto &simul_config = sim_config.sim;

        // Movement settings
        if (ImGui::TreeNode("Movement")) {
            config_changed |= ImGui::DragFloat("Min Speed", &simul_config.speed_min, 0.0001F,
                                               0.0001F, 0.01F, "%.4f");
            config_changed |= ImGui::DragFloat("Max Speed", &simul_config.speed_max, 0.0001F,
                                               0.001F, 0.05F, "%.4f");
            config_changed |= ImGui::DragFloat("Acceleration", &simul_config.speed_accel, 0.01F,
                                               0.01F, 0.5F, "%.3f");
            config_changed |= ImGui::DragFloat("Turn Rate", &simul_config.rotation_accel_deg, 1.0F,
                                               1.0F, 90.0F, "%.3f");
            ImGui::TreePop();
        }

        if (config_changed) {
            config_.set_simulation(sim_config);
            reset_world();
        }
        ImGui::TreePop();
    }

    // Brain configuration
    if (ImGui::TreeNode("Brain-Eye Config")) {
        bool config_changed = false;
        auto sim_config = config_.get_simulation();
        auto &brain_eye_config = sim_config.brain_eye;
        // Eye configuration
        config_changed |=
            ImGui::SliderFloat("Eye FOV", &brain_eye_config.fov_angle_deg, 5.0F, 360.0F);
        config_changed |= ImGui::SliderFloat("Eye Range", &brain_eye_config.fov_range, 0.1F, 1.0F);
        config_changed |= ImGui::SliderInt(
            "Eye Cells", reinterpret_cast<int *>(&brain_eye_config.num_cells), 1, 16);

        config_changed |= ImGui::SliderInt(
            "Output Neurons", reinterpret_cast<int *>(&brain_eye_config.num_neurons), 1, 16);

        if (config_changed) {
            config_.set_simulation(sim_config);
            reset_world();
        }
        ImGui::TreePop();
    }

    // Genetic configuration
    if (ImGui::TreeNode("Genetic Config")) {
        bool config_changed = false;
        auto sim_config = config_.get_simulation();
        auto &genetic_config = sim_config.genetic;

        config_changed |=
            ImGui::SliderFloat("Mutation Chance", &genetic_config.mutation_chance, 0.001F, 0.5F);
        config_changed |=
            ImGui::SliderFloat("Mutation Coeff", &genetic_config.mutation_coeff, 0.01F, 1.0F);

        if (config_changed) {
            config_.set_simulation(sim_config);
            reset_world();
        }
        ImGui::TreePop();
    }
    ImGui::PopItemWidth();
}

void simulation_window::render_statistics() {
    ImGui::Text("Generation: %zu", simulation_->get_generation());
    ImGui::Text("Time: %.1f s", elapsed_time_);
    ImGui::Text("Best Fitness: %zu", best_fitness_);
    ImGui::Text("Average Fitness: %.2f", avg_fitness_);
}

std::string simulation_window::train(size_t generations) {
    if (evolution_in_progress_) {
        return "Evolution already in progress";
    }

    evolution_in_progress_ = true;
    const bool was_paused = paused_;
    paused_ = false;

    std::string result;

    try {
        // Run the simulation for specified number of generations
        for (size_t gen = 0; gen < generations; ++gen) {
            // Train the simulation - equivalent to Rust's let stats = sim.train(&mut rng);
            auto stats = simulation_->train(random_);

            // Add a newline between generations (except for the first one)
            if (gen > 0) {
                result += "\n";
            }

            // Add the stats for this generation directly
            result += stats.to_string();

            // Update statistics
            best_fitness_ = static_cast<size_t>(stats.ga_stats().max_fitness());
            avg_fitness_ = stats.ga_stats().avg_fitness();

            spdlog::info("Generation {}: {}", simulation_->get_generation(), stats.to_string());
        }
    } catch (const std::exception &e) {
        spdlog::error("Training error: {}", e.what());
        result += "\nError: " + std::string(e.what());
    }

    // Restore pause state
    paused_ = was_paused;
    evolution_in_progress_ = false;

    return result;
}

void simulation_window::render_config_controls() {
    // Calculate consistent width for input components
    const float content_width = ImGui::GetContentRegionAvail().x;
    const float input_width = content_width * 0.65f;  // Use 65% of available width for inputs
    const float label_width = content_width * 0.35f;  // Use 35% for labels

    // File path input
    static char config_path_buffer[256] = "";

    // If this is the first run or the config path has changed, update the buffer
    if (strlen(config_path_buffer) == 0 && !config_path_.empty()) {
        strncpy(config_path_buffer, config_path_.c_str(), sizeof(config_path_buffer) - 1);
        config_path_buffer[sizeof(config_path_buffer) - 1] = '\0';
    }

    // Show a better label above the path
    ImGui::Text("Config Path:");

    // Create a button layout with a read-only text field and browse button
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, ImGui::GetStyle().ItemSpacing.y));

    // Read-only text field
    ImGui::PushItemWidth(content_width - 104);  // Make space for the button
    ImGui::InputText("##ConfigPath", config_path_buffer, sizeof(config_path_buffer),
                     ImGuiInputTextFlags_ReadOnly);
    ImGui::PopItemWidth();

    ImGui::SameLine();
    if (ImGui::Button("Browse", ImVec2(100, 0))) {
        ifd::FileDialog::Instance().Open("ConfigPathDlg", "Choose Config File",
                                         "Config Files (*.json){.json},.*", false,
                                         default_config_path_);
    }
    ImGui::PopStyleVar();

    // Process file dialog result
    if (ifd::FileDialog::Instance().IsDone("ConfigPathDlg")) {
        if (ifd::FileDialog::Instance().HasResult()) {
            std::filesystem::path result = ifd::FileDialog::Instance().GetResult();
            config_path_ = result.string();
            strncpy(config_path_buffer, config_path_.c_str(), sizeof(config_path_buffer) - 1);
            config_path_buffer[sizeof(config_path_buffer) - 1] = '\0';
            spdlog::info("Selected config path: {}", config_path_);
        }
        ifd::FileDialog::Instance().Close();
    }

    ImGui::Spacing();

    // Load/Save buttons
    if (ImGui::Button("Load Config")) {
        if (load_config(config_path_buffer)) {
            spdlog::info("Configuration loaded successfully");
        }
    }
    ImGui::SameLine();
    if (ImGui::Button("Save Config")) {
        if (save_config(config_path_buffer)) {
            spdlog::info("Configuration saved successfully");
        }
    }

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    // World configuration
    if (ImGui::TreeNode("World Settings")) {
        bool config_changed = false;
        auto sim_config = config_.get_simulation();
        auto &world_config = sim_config.world;

        ImGui::PushItemWidth(input_width);
        config_changed |= ImGui::DragInt(
            "Food Count", reinterpret_cast<int *>(&world_config.num_foods), 1, 10, 1000);
        config_changed |= ImGui::DragFloat("Food Size", &world_config.food_size, 0.1F, 0.5F, 10.0F);
        config_changed |= ImGui::DragInt(
            "Initial Birds", reinterpret_cast<int *>(&world_config.num_animals), 1, 10, 1000);
        config_changed |= ImGui::DragFloat("Bird Size", &world_config.bird_size, 0.1F, 1.0F, 50.0F);
        ImGui::PopItemWidth();

        if (config_changed) {
            config_.set_simulation(sim_config);
            reset_world();
        }
        ImGui::TreePop();
    }

    // Animal configuration
    if (ImGui::TreeNode("Animal Settings")) {
        bool config_changed = false;
        auto sim_config = config_.get_simulation();
        auto &animal_config = sim_config.sim;
        auto &eye_config = sim_config.brain_eye;

        // Movement settings
        if (ImGui::TreeNode("Movement")) {
            ImGui::PushItemWidth(input_width);
            config_changed |= ImGui::DragFloat("Min Speed", &animal_config.speed_min, 0.001F,
                                               0.0001F, 0.01F, "%.4f");
            config_changed |= ImGui::DragFloat("Max Speed", &animal_config.speed_max, 0.001F,
                                               0.001F, 0.05F, "%.4f");
            config_changed |= ImGui::DragFloat("Acceleration", &animal_config.speed_accel, 0.01F,
                                               0.01F, 0.5F, "%.3f");
            ImGui::PopItemWidth();
            ImGui::TreePop();
        }

        // Eye settings
        if (ImGui::TreeNode("Eye")) {
            ImGui::PushItemWidth(input_width);
            config_changed |= ImGui::DragFloat("FOV angle(º)", &eye_config.fov_angle_deg, 0.1F,
                                               0.1F, 2.0F * simulation::constants::k_pi);
            config_changed |=
                ImGui::DragFloat("View Range", &eye_config.fov_range, 0.1F, 1.0F, 100.0F);
            config_changed |=
                ImGui::DragInt("Cells", reinterpret_cast<int *>(&eye_config.num_cells), 1, 1, 32);
            ImGui::PopItemWidth();
            ImGui::TreePop();
        }

        // Brain settings
        if (ImGui::TreeNode("Brain")) {
            ImGui::PushItemWidth(input_width);
            config_changed |= ImGui::DragInt(
                "Neurons", reinterpret_cast<int *>(&eye_config.num_neurons), 1, 1, 16);
            ImGui::PopItemWidth();
            ImGui::TreePop();
        }

        if (config_changed) {
            config_.set_simulation(sim_config);
            reset_world();
        }
        ImGui::TreePop();
    }

    // Genetic configuration
    if (ImGui::TreeNode("Genetic Settings")) {
        bool config_changed = false;
        auto sim_config = config_.get_simulation();
        auto &genetic_config = sim_config.genetic;

        ImGui::PushItemWidth(input_width);
        config_changed |= ImGui::DragFloat("Mutation Chance", &genetic_config.mutation_chance,
                                           0.001F, 0.0F, 1.0F, "%.3f");
        config_changed |= ImGui::DragFloat("Mutation Coefficient", &genetic_config.mutation_coeff,
                                           0.01F, 0.0F, 1.0F, "%.2f");

        // Generation length is a float in our UI but size_t in backend, handle carefully
        auto generation_length = static_cast<float>(sim_config.sim.generation_length);
        if (ImGui::DragFloat("Generation Length", &generation_length, 1.0F, 1.0F, 120.0F)) {
            sim_config.sim.generation_length = static_cast<size_t>(generation_length);
            config_changed = true;
        }
        ImGui::PopItemWidth();

        if (config_changed) {
            config_.set_simulation(sim_config);
            reset_world();
        }
        ImGui::TreePop();
    }

    // UI configuration
    if (ImGui::TreeNode("UI Settings")) {
        bool config_changed = false;
        auto ui_config = config_.get_ui();

        ImGui::PushItemWidth(input_width);
        config_changed |= ImGui::Checkbox("Show Vision Cones", &ui_config.show_vision_cones);
        ImGui::PopItemWidth();

        if (config_changed) {
            config_.set_ui(ui_config);
        }
        ImGui::TreePop();
    }
}

void simulation_window::render_console() {
    const float footer_height =
        ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();

    // Shorelark ASCII art banner
    ImGui::TextColored(ImVec4(0.5F, 0.7F, 1.0F, 1.0F),
                       "   _____  _____ _                    _            _    \n"
                       "  / ____|/ ____| |                  | |          | |   \n"
                       " | |    | (___ | |__   ___  _ __ ___| | __ _ _ __| | __\n"
                       " | |     \\___ \\| '_ \\ / _ \\| '__/ _ \\ |/ _` | '__| |/ /\n"
                       " | |____ ____) | | | | (_) | | |  __/ | (_| | |  |   < \n"
                       "  \\_____|_____/|_| |_|\\___/|_|  \\___|_|\\__,_|_|  |_|\\_\\\n"
                       "                                                       \n"
                       "                                                       ");
    ImGui::Spacing();
    ImGui::TextColored(ImVec4(0.9F, 0.9F, 0.9F, 1.0F), "Welcome to CShorelark C++ Simulation");
    ImGui::Text("Neural network driven bird simulation with evolutionary learning");
    ImGui::Spacing();

    // Create console output area
    ImGui::BeginChild("ScrollingRegion", ImVec2(0, -footer_height), false,
                      ImGuiWindowFlags_HorizontalScrollbar);

    // Display history
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 1));
    for (const auto &line : console_history_) {
        ImGui::TextUnformatted(line.c_str());
    }

    // Auto-scroll
    if (console_scroll_to_bottom_ || (ImGui::GetScrollY() >= ImGui::GetScrollMaxY())) {
        ImGui::SetScrollHereY(1.0F);
    }
    console_scroll_to_bottom_ = false;
    ImGui::PopStyleVar();
    ImGui::EndChild();
    ImGui::Separator();

    // Command input field
    ImGui::PushItemWidth(-1);
    bool reclaim_focus = false;
    if (ImGui::InputText("##command", &console_input_buffer_,
                         ImGuiInputTextFlags_EnterReturnsTrue)) {
        if (!console_input_buffer_.empty()) {
            // Add to history
            console_history_.emplace_back("> " + console_input_buffer_);

            // Parse command and arguments
            std::istringstream iss(console_input_buffer_);
            std::string command;
            iss >> command;

            // Handle commands
            try {
                if (command == "help") {
                    console_history_.emplace_back("Available commands:");
                    console_history_.emplace_back("  help - Show this help");
                    console_history_.emplace_back("  reset - Reset the simulation");
                    console_history_.emplace_back(
                        "  train [generations] - Run training for one or more generations");
                    console_history_.emplace_back("  pause / play - Toggle simulation pause state");
                    console_history_.emplace_back(
                        "  spawn animal - Add a new animal to the simulation");
                    console_history_.emplace_back("  spawn food - Add new food to the simulation");
                } else if (command == "reset") {
                    reset_world();
                    console_history_.emplace_back("Simulation reset.");
                } else if (command == "t" || command == "train") {
                    // Parse generations parameter if provided
                    int generations = 1;  // Default to 1 generation
                    iss >> generations;

                    if (generations <= 0) {
                        console_history_.emplace_back(
                            "Error: Generations must be a positive number");
                    } else {
                        auto result = train(static_cast<size_t>(generations));
                        console_history_.emplace_back(result);
                    }
                } else if (command == "p" || command == "pause" || command == "play") {
                    paused_ = !paused_;
                    console_history_.emplace_back(paused_ ? "Simulation paused."
                                                          : "Simulation resumed.");
                } else if (command == "spawn") {
                    std::string entity;
                    iss >> entity;

                    if (entity == "animal") {
                        spawn_animal();
                        console_history_.emplace_back("New animal spawned.");
                    } else if (entity == "food") {
                        spawn_food();
                        console_history_.emplace_back("New food spawned.");
                    } else {
                        console_history_.emplace_back(
                            "Unknown entity type. Try 'spawn animal' or 'spawn food'.");
                    }
                } else {
                    console_history_.emplace_back(
                        "Unknown command. Type 'help' for available commands.");
                }
            } catch (const std::exception &e) {
                console_history_.emplace_back(std::string("Error: ") + e.what());
            }

            // Clear input and scroll
            console_input_buffer_.clear();
            console_scroll_to_bottom_ = true;
        }
        reclaim_focus = true;
    }
    ImGui::PopItemWidth();

    // Auto-focus on the input box
    if (reclaim_focus) {
        ImGui::SetKeyboardFocusHere(-1);
    }
}

auto simulation_window::load_config(const std::filesystem::path &path) -> bool {
    if (auto result = simulation_window_config::load_from_file(path)) {
        config_path_ = path.string();
        apply_config(*result);
        return true;
    }
    return false;
}

auto simulation_window::save_config(const std::filesystem::path &path) const -> bool {
    if (auto result = config_.save_to_file(path)) {
        // We can't modify config_path_ here since it's const, but that's fine
        // as it will still save to the correct location
        return true;
    }
    return false;
}

void simulation_window::apply_config(const simulation_window_config &config) {
    config_ = config;
    // Recreate world with new configuration
    reset_world();
}

// Copy relevant data from simulation to GUI data structure for rendering
void simulation_window::update_data() {
    if (!simulation_)
        return;

    // Clear previous GUI data
    gui_data_.birds.clear();
    gui_data_.foods.clear();

    // Copy animals to GUI birds
    const auto &animals = simulation_->get_world().get_animals();
    gui_data_.birds.reserve(animals.size());
    size_t fitness_sum = 0;
    best_fitness_ = 0;
    for (const auto &animal : animals) {
        gui_bird bird;
        bird.pos_x = animal.position().x();
        bird.pos_y = animal.position().y();
        bird.rotation = animal.rotation();
        bird.speed = animal.speed();
        bird.fitness = animal.food_eaten();
        gui_data_.birds.push_back(bird);
        size_t fitness = bird.fitness;
        if (fitness > best_fitness_) best_fitness_ = fitness;
        fitness_sum += fitness;
    }

    // Copy foods to GUI foods
    const auto &foods = simulation_->get_world().get_foods();
    gui_data_.foods.reserve(foods.size());
    for (const auto &food : foods) {
        gui_food f;
        f.pos_x = food.position().x();
        f.pos_y = food.position().y();
        gui_data_.foods.push_back(f);
    }

    // Update statistics
    avg_fitness_ = animals.empty() ? 0.0f : static_cast<float>(fitness_sum) / animals.size();
}

// Start the simulation thread
void simulation_window::start_simulation_thread() {
    spdlog::debug("Starting simulation thread");

    // Make sure we don't have an existing thread
    stop_simulation_thread();

    // Reset the exit flag
    thread_should_exit_ = false;

    // Start the thread
    simulation_thread_ = std::thread(&simulation_window::simulation_thread_function, this);

    spdlog::debug("Simulation thread started");
}

// Stop the simulation thread safely
void simulation_window::stop_simulation_thread() {
    if (simulation_thread_.joinable()) {
        spdlog::debug("Stopping simulation thread");

        // Signal thread to exit
        thread_should_exit_ = true;

        // Wait for thread to finish
        simulation_thread_.join();

        spdlog::debug("Simulation thread stopped");
    }
}

// The main simulation thread function
void simulation_window::simulation_thread_function() {
    spdlog::debug("Simulation thread function started");

    // Use high-resolution clock for accurate timing
    using clock_type = std::chrono::steady_clock;
    last_step_time_ = clock_type::now();

    // Main thread loop
    while (!thread_should_exit_) {
        // Check if simulation is paused
        if (!paused_) {
            // Calculate elapsed time since last step
            auto current_time = clock_type::now();
            float elapsed_seconds =
                std::chrono::duration<float>(current_time - last_step_time_).count();
            last_step_time_ = current_time;

            // Apply simulation speed factor
            float simulation_speed = config_.get_ui().simulation_speed;
            float dt = elapsed_seconds * simulation_speed;

            // Ensure dt is reasonable for numerical stability
            constexpr float max_dt = 0.05F;  // Max 50ms steps
            if (dt > max_dt) {
                // Break into smaller steps
                int steps = static_cast<int>(std::ceil(dt / max_dt));

                for (int i = 0; i < steps && !paused_ && !thread_should_exit_; i++) {
                    simulation_->step(random_);
                }
            } else {
                // Single step
                simulation_->step(random_);
            }

            // Update elapsed time counter
            elapsed_time_ += dt;

            // Update GUI data with a thread-safe approach
            {
                const std::lock_guard<std::mutex> lock(gui_data_mutex_);
                update_data();
                gui_data_updated_ = true;
            }
        }

        // Sleep to avoid consuming too much CPU
        // Adjust this value based on desired simulation responsiveness vs. CPU usage
        std::this_thread::sleep_for(std::chrono::milliseconds(min_sleep_time_ms));
    }

    spdlog::debug("Simulation thread function exiting");
}

}  // namespace cshorelark
