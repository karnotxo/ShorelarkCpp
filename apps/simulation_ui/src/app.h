#ifndef CSHORELARK_APPS_SIMULATION_UI_APP_H
#define CSHORELARK_APPS_SIMULATION_UI_APP_H

#include <memory>
#include <string>
#include <string_view>
#include <functional>
#include <vector>

#include "imgui_context.h"

struct GLFWwindow;

namespace cshorelark {

/**
 * @brief Main application class handling window and ImGui initialization.
 */
class App {
public:
    /**
     * @brief Constructs a new App with the given title and dimensions.
     * @param title The window title
     * @param width The window width in pixels
     * @param height The window height in pixels
     */
    explicit App(std::string_view title, int width = 1280, int height = 720);
    ~App();

    // Delete copy operations
    App(const App&) = delete;
    App& operator=(const App&) = delete;

    // Allow move operations
    App(App&&) noexcept;
    App& operator=(App&&) noexcept;

    /**
     * @brief Checks if the window should close.
     * @return true if the window should close, false otherwise
     */
    [[nodiscard]] bool ShouldClose() const noexcept;

    /**
     * @brief Runs a single frame of the application.
     * @param render_callback Optional callback function to render content before the frame ends
     */
    void RunFrame(const std::function<void()>& render_callback = nullptr);

    /**
     * @brief Gets the ImGui context for UI rendering.
     * @return Reference to the ImGui context
     */
    [[nodiscard]] ImGuiContext& GetImGuiContext() { return *imgui_context_; }

    /**
     * @brief Gets the GLFW window handle.
     * @return Pointer to the GLFW window
     */
    [[nodiscard]] GLFWwindow* GetWindow() const noexcept { return window_; }

private:
    void InitGlfw();
    void InitGlew();
    void SetWindowIcon();

    GLFWwindow* window_;
    std::string title_;
    int width_;
    int height_;
    std::unique_ptr<ImGuiContext> imgui_context_;
    
    // Icon data
    struct IconData {
        std::vector<unsigned char> pixels;
        int width = 0;
        int height = 0;
    };
    std::vector<IconData> icon_data_;
};
}  // namespace cshorelark

#endif  // CSHORELARK_APPS_SIMULATION_UI_APP_H