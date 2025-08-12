#ifndef CSHORELARK_APPS_SIMULATION_UI_SIMULATION_WINDOW_H
#define CSHORELARK_APPS_SIMULATION_UI_SIMULATION_WINDOW_H

#include <transwarp.h>

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <filesystem>
#include <memory>
#include <mutex>
#include <nonstd/span.hpp>
#include <string>
#include <thread>  // Added for hardware_concurrency
#include <vector>

#include "random/random.h"
#include "simulation/animal.h"
#include "simulation/simulation.h"
#include "simulation/world.h"
#include "simulation_config.h"

namespace cshorelark {

// GUI representations of simulation entities
struct gui_bird {
    float pos_x;
    float pos_y;
    float rotation;
    float speed;
    size_t fitness;
};

struct gui_food {
    float pos_x;
    float pos_y;
};

struct gui_world_data {
    std::vector<gui_bird> birds;
    std::vector<gui_food> foods;
    size_t generation;
    float best_fitness;
    float avg_fitness;
};

/**
 * @brief Main window for the simulation visualization and control.
 */
class simulation_window {
public:
    /**
     * @brief Constructs a new simulation window with default configuration.
     */
    explicit simulation_window();

    /**
     * @brief Destructor to ensure the simulation thread is properly stopped.
     */
    ~simulation_window();

    // Delete copy operations
    simulation_window(const simulation_window&) = delete;
    simulation_window& operator=(const simulation_window&) = delete;

    // Allow move operations
    simulation_window(simulation_window&&) noexcept = default;
    simulation_window& operator=(simulation_window&&) noexcept = default;

    /**
     * @brief Renders the simulation window and all its components.
     */
    void render();

    // Configuration management
    /**
     * @brief Loads simulation configuration from a file.
     * @param path Path to the configuration file
     * @return true if loading was successful, false otherwise
     */
    [[nodiscard]] auto load_config(const std::filesystem::path& path) -> bool;

    /**
     * @brief Saves current simulation configuration to a file.
     * @param path Path where to save the configuration
     * @return true if saving was successful, false otherwise
     */
    [[nodiscard]] auto save_config(const std::filesystem::path& path) const -> bool;

    /**
     * @brief Applies a new configuration to the simulation.
     * @param config The configuration to apply
     */
    void apply_config(const simulation_window_config& config);

    /**
     * @brief Gets the current simulation configuration.
     * @return Reference to the current configuration
     */
    [[nodiscard]] const simulation_window_config& get_config() const { return config_; }

    // Getters for testing and monitoring
    [[nodiscard]] auto get_animal_count() const -> size_t {
        return simulation_ ? simulation_->get_world().get_animals().size() : 0;
    }
    [[nodiscard]] auto get_world_config() const -> const simulation::world_config& {
        return config_.get_simulation().world;
    }
    [[nodiscard]] float get_elapsed_time() const { return elapsed_time_; }
    [[nodiscard]] size_t get_generation() const { return simulation_->get_generation(); }
    [[nodiscard]] float get_best_fitness() const { return best_fitness_; }
    [[nodiscard]] float get_average_fitness() const { return avg_fitness_; }
    [[nodiscard]] bool is_paused() const { return paused_; }
    [[nodiscard]] float get_simulation_speed() const { return config_.get_ui().simulation_speed; }

    // Control methods
    void set_paused(bool paused) { paused_ = paused; }
    void set_simulation_speed(float speed) {
        auto ui_cfg = config_.get_ui();
        ui_cfg.simulation_speed = speed;
        config_.set_ui(ui_cfg);
    }
    [[nodiscard]] auto train(size_t generations = 1) -> std::string;

private:
    void reset_world();
    void render_world();
    void render_controls();
    void render_statistics();
    void render_config_controls();
    void render_console();
    void spawn_animal();
    void spawn_food();
    void update_data();

    // Thread management methods
    void start_simulation_thread();
    void stop_simulation_thread();
    void simulation_thread_function();

    std::shared_ptr<transwarp::task<void>> create_batch_task(size_t batch_start, size_t batch_size);

    simulation_window_config config_;
    std::string config_path_;  ///< Path to last loaded/saved config
    std::unique_ptr<simulation::simulation> simulation_;
    cshorelark::random::random_generator random_;

    // Task graph related members
    std::shared_ptr<transwarp::task<void>> update_task_;
    std::vector<std::shared_ptr<transwarp::task<void>>> batch_tasks_;
    transwarp::parallel executor_{
        std::thread::hardware_concurrency()};  // Initialize with number of CPU cores
    size_t last_population_size_{0};

    // Evolution state
    float elapsed_time_{0.0F};
    size_t best_fitness_{0};
    float avg_fitness_{0.0F};
    std::atomic<bool> paused_{false};
    std::atomic<bool> evolution_in_progress_{false};

    // Threading support
    std::thread simulation_thread_;
    std::atomic<bool> thread_should_exit_{false};
    std::mutex gui_data_mutex_;
    std::condition_variable simulation_cv_;
    std::chrono::steady_clock::time_point last_step_time_;

    // GUI state
    gui_world_data gui_data_;
    std::atomic<bool> gui_data_updated_{false};
    float step_interval_{0.016F};  // Target 60 FPS as base rate

    // Console state
    std::string console_input_buffer_;
    std::vector<std::string> console_history_;
    bool console_scroll_to_bottom_{true};

    // File dialog state
    bool file_dialog_open_{false};
    std::filesystem::path default_config_path_;
};

}  // namespace cshorelark

#endif  // CSHORELARK_APPS_SIMULATION_UI_SIMULATION_WINDOW_H