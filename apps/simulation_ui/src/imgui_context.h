#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <string>

#include "imgui.h"

struct GLFWwindow;

namespace cshorelark {

/**
 * @brief RAII wrapper for ImGui context and initialization.
 *
 * This class handles ImGui context creation, initialization, and cleanup.
 * It also provides utility functions for common ImGui operations.
 */
class ImGuiContext {
public:
    /**
     * @brief Initializes ImGui context and sets up OpenGL/GLFW backends.
     * @param window GLFW window to bind ImGui to
     * @param glsl_version GLSL version string (e.g., "#version 130")
     */
    explicit ImGuiContext(GLFWwindow* window, const std::string& glsl_version = "#version 130");
    ~ImGuiContext();

    // Disable copy and assignment
    ImGuiContext(const ImGuiContext&) = delete;
    ImGuiContext& operator=(const ImGuiContext&) = delete;

    // Allow move semantics
    ImGuiContext(ImGuiContext&&) noexcept;
    ImGuiContext& operator=(ImGuiContext&&) noexcept;

    /**
     * @brief Starts a new ImGui frame.
     */
    void new_frame();

    /**
     * @brief Renders the current ImGui frame.
     */
    void render();

    /**
     * @brief Sets up the default dark style with modern tweaks.
     */
    void SetupStyle();

    /**
     * @brief Begins a new window with common settings.
     * @param name Window name/ID
     * @param open Pointer to boolean controlling window visibility
     * @param flags ImGui window flags
     * @return true if window is visible and ready to be filled
     */
    [[nodiscard]] bool BeginWindow(const char* name, bool* open = nullptr, int flags = 0);

    /**
     * @brief Ends a window previously started with BeginWindow.
     */
    void EndWindow();

    /**
     * @brief Setup the simulation rendering buffer
     * @param width Initial width of the buffer
     * @param height Initial height of the buffer
     */
    void SetupSimulationBuffer(int width, int height);

    /**
     * @brief Begin rendering to the simulation framebuffer
     * @param clear_color Optional clear color for the framebuffer
     */
    void BeginSimulationRendering(const float clear_color[4] = nullptr);

    /**
     * @brief End rendering to the simulation framebuffer
     */
    void EndSimulationRendering();

    /**
     * @brief Display the simulation texture in an ImGui window
     * @return true if the window was displayed and interaction is possible
     */
    bool ShowSimulationWindow();

    /**
     * @brief Get the current simulation window size
     * @return ImVec2 containing width and height
     */
    ImVec2 GetSimulationWindowSize() const;

private:
    GLFWwindow* window_;
    bool initialized_ = false;

    // Framebuffer objects for rendering simulation
    GLuint sim_framebuffer_ = 0;
    GLuint sim_texture_ = 0;
    GLuint sim_depth_buffer_ = 0;
    int sim_buffer_width_ = 1024;
    int sim_buffer_height_ = 768;
    ImVec2 sim_window_size_ = ImVec2(0, 0);

    // Internal helpers
    void InitBackends(GLFWwindow* window, std::string_view glsl_version);
    void CleanupSimulationBuffer();
    void ResizeSimulationBufferIfNeeded(int width, int height);

    ImVec2 PrepareSimulationViewport(const float clear_color[4]);
    bool BeginSimulationViewWindow();
    // Static helper to cleanup ImGui backends - ensures consistent cleanup order
    static void cleanup_backends() noexcept;
};

}  // namespace cshorelark