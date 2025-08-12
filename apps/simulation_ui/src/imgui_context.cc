#include "imgui_context.h"

#include <imgui_internal.h>
#include <spdlog/spdlog.h>

#include <stdexcept>

#include "ImFileDialog.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

namespace cshorelark {

ImGuiContext::ImGuiContext(GLFWwindow* window, const std::string& glsl_version) : window_(window) {
    spdlog::debug("Creating ImGui context with GLSL version {}", glsl_version);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    spdlog::debug("ImGui context created");

    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable keyboard controls
    // Comment out docking - we don't need it anymore
    // io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;      // Enable docking
    spdlog::debug("ImGui keyboard navigation enabled");

    // Enable viewports for multi-window support (available in 1.91.8-docking)
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
    spdlog::debug("ImGui viewports enabled");

    // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows
    // can look identical to regular ones
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
        ImGuiStyle& style = ImGui::GetStyle();
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    spdlog::debug("ImGui dark style set");

    // Initialize backend
    InitBackends(window, glsl_version);

    // ImFileDialog requires you to set the CreateTexture and DeleteTexture
    ifd::FileDialog::Instance().CreateTexture = [](uint8_t* data, int w, int h, char fmt) -> void* {
        GLuint tex;

        glGenTextures(1, &tex);
        glBindTexture(GL_TEXTURE_2D, tex);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, (fmt == 0) ? GL_BGRA : GL_RGBA,
                     GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, 0);

        return (void*)tex;
    };
    ifd::FileDialog::Instance().DeleteTexture = [](void* tex) {
        GLuint texID = (GLuint)((uintptr_t)tex);
        glDeleteTextures(1, &texID);
    };

    initialized_ = true;
    spdlog::info("ImGui context fully initialized");
}

ImGuiContext::~ImGuiContext() {
    spdlog::debug("Destroying ImGui context");
    if (initialized_) {
        CleanupSimulationBuffer();
        cleanup_backends();
        spdlog::trace("ImGui backends cleaned up");

        ImGui::DestroyContext();
        spdlog::trace("ImGui context destroyed");
    }
    spdlog::debug("ImGui context cleanup complete");
}

ImGuiContext::ImGuiContext(ImGuiContext&& other) noexcept
    : window_(other.window_), initialized_(other.initialized_) {
    spdlog::debug("ImGui context move constructor");
    other.initialized_ = false;
}

ImGuiContext& ImGuiContext::operator=(ImGuiContext&& other) noexcept {
    spdlog::debug("ImGui context move assignment");
    if (this != &other) {
        // Clean up current resources
        if (initialized_) {
            cleanup_backends();
            ImGui::DestroyContext();
            spdlog::trace("Existing ImGui context destroyed during move");
        }

        // Transfer ownership
        window_ = other.window_;
        initialized_ = other.initialized_;
        other.initialized_ = false;
    }
    return *this;
}

void ImGuiContext::new_frame() {
    spdlog::trace("Starting new ImGui frame");
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void ImGuiContext::render() {
    spdlog::trace("Rendering ImGui frame");
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    // Update and Render additional Platform Windows
    // (Platform functions may change the current OpenGL context, so we save/restore it)
    ImGuiIO& io = ImGui::GetIO();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
        GLFWwindow* backup_current_context = glfwGetCurrentContext();
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        glfwMakeContextCurrent(backup_current_context);
        spdlog::trace("ImGui platform windows rendered");
    }
}

void ImGuiContext::SetupStyle() {
    spdlog::debug("Setting up ImGui custom style");
    ImGui::StyleColorsDark();
    ImGuiStyle& style = ImGui::GetStyle();

    // Round the corners
    style.WindowRounding = 5.0f;
    style.ChildRounding = 3.0f;
    style.FrameRounding = 3.0f;
    style.PopupRounding = 3.0f;
    style.ScrollbarRounding = 3.0f;
    style.GrabRounding = 3.0f;
    style.TabRounding = 3.0f;

    // Adjust spacing
    style.ItemSpacing = ImVec2(8, 4);
    style.FramePadding = ImVec2(6, 3);
    style.WindowPadding = ImVec2(8, 8);

    // Modern colors
    ImVec4* colors = style.Colors;
    colors[ImGuiCol_WindowBg] = ImVec4(0.1f, 0.1f, 0.1f, 1.0f);
    colors[ImGuiCol_Border] = ImVec4(0.3f, 0.3f, 0.3f, 0.5f);
    colors[ImGuiCol_FrameBg] = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);
    colors[ImGuiCol_TitleBg] = ImVec4(0.15f, 0.15f, 0.15f, 1.0f);
    colors[ImGuiCol_TitleBgActive] = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);
    colors[ImGuiCol_Button] = ImVec4(0.3f, 0.3f, 0.3f, 1.0f);
    colors[ImGuiCol_ButtonHovered] = ImVec4(0.4f, 0.4f, 0.4f, 1.0f);
    colors[ImGuiCol_ButtonActive] = ImVec4(0.5f, 0.5f, 0.5f, 1.0f);
    colors[ImGuiCol_Header] = ImVec4(0.25f, 0.25f, 0.25f, 1.0f);
    colors[ImGuiCol_HeaderHovered] = ImVec4(0.35f, 0.35f, 0.35f, 1.0f);
    colors[ImGuiCol_HeaderActive] = ImVec4(0.45f, 0.45f, 0.45f, 1.0f);
    colors[ImGuiCol_Tab] = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);
    colors[ImGuiCol_TabHovered] = ImVec4(0.3f, 0.3f, 0.3f, 1.0f);
    colors[ImGuiCol_TabActive] = ImVec4(0.35f, 0.35f, 0.35f, 1.0f);
    spdlog::debug("ImGui style setup complete");
}

bool ImGuiContext::BeginWindow(const char* name, bool* open, int flags) {
    spdlog::trace("Begin window '{}'", name);
    return ImGui::Begin(name, open, flags);
}

void ImGuiContext::EndWindow() { ImGui::End(); }

bool ImGuiContext::BeginSimulationViewWindow() {
    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);

    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar |
                                    ImGuiWindowFlags_NoScrollWithMouse |
                                    ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
                                    ImGuiWindowFlags_NoMove | ImGuiWindowFlags_MenuBar;

    // Create a very simple window for rendering
    bool open = ImGui::Begin("Simulation View", nullptr, window_flags);
    spdlog::trace("Simulation view window opened");

    return open;
}

ImVec2 ImGuiContext::PrepareSimulationViewport(const float clear_color[4]) {
    // Get the window position and size
    ImVec2 pos = ImGui::GetCursorScreenPos();
    ImVec2 size = ImGui::GetContentRegionAvail();

    // Only render if we have a valid size
    if (size.x <= 0 || size.y <= 0) {
        spdlog::warn("Invalid simulation viewport size: {}x{}", size.x, size.y);
        return size;
    }

    // Set the viewport to match the ImGui window content area
    glViewport(pos.x, pos.y, size.x, size.y);

    // Clear the viewport with the specified color or default
    if (clear_color) {
        glClearColor(clear_color[0], clear_color[1], clear_color[2], clear_color[3]);
    } else {
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);  // Default to black
    }
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Draw a simple test pattern so we can see it's working
    float time = static_cast<float>(glfwGetTime());

    // Simple pulsing color
    float pulse = (sinf(time * 2.0f) * 0.5f + 0.5f);

    // Draw a simple triangle as a test pattern
    glBegin(GL_TRIANGLES);
    glColor3f(pulse, 0.0f, 1.0f - pulse);
    glVertex2f(-0.5f, -0.5f);
    glColor3f(0.0f, 1.0f, 0.0f);
    glVertex2f(0.5f, -0.5f);
    glColor3f(0.0f, 0.0f, 1.0f);
    glVertex2f(0.0f, 0.5f);
    glEnd();

    return size;
}

void ImGuiContext::InitBackends(GLFWwindow* window, std::string_view glsl_version) {
    spdlog::debug("Initializing ImGui backends for window at {} with GLSL {}",
                  static_cast<void*>(window), glsl_version);
    bool glfw_init_success = ImGui_ImplGlfw_InitForOpenGL(window, true);
    bool opengl3_init_success = ImGui_ImplOpenGL3_Init(std::string(glsl_version).c_str());

    if (!glfw_init_success || !opengl3_init_success) {
        spdlog::critical("Failed to initialize ImGui backends: GLFW={}, OpenGL3={}",
                         glfw_init_success, opengl3_init_success);
        throw std::runtime_error("Failed to initialize ImGui backend");
    }
    spdlog::debug("ImGui backends initialized successfully");
}

void ImGuiContext::cleanup_backends() noexcept {
    spdlog::debug("Cleaning up ImGui backends");
    ImGui_ImplOpenGL3_Shutdown();
    spdlog::trace("ImGui OpenGL3 backend shutdown");

    ImGui_ImplGlfw_Shutdown();
    spdlog::trace("ImGui GLFW backend shutdown");
}

void ImGuiContext::SetupSimulationBuffer(int width, int height) {
    // Simplified to do minimal setup or nothing
    spdlog::debug("Simulation buffer simplified, using direct rendering");
}

void ImGuiContext::CleanupSimulationBuffer() {
    // Simplified cleanup
    spdlog::debug("Simulation buffer cleanup (simplified)");
}

void ImGuiContext::ResizeSimulationBufferIfNeeded(int width, int height) {
    if (width <= 0 || height <= 0) {
        return;  // Invalid size
    }

    // Only resize if dimensions changed significantly (avoid constant resizing)
    if (std::abs(width - sim_buffer_width_) > 10 || std::abs(height - sim_buffer_height_) > 10) {
        spdlog::debug("Resizing simulation framebuffer to {} x {}", width, height);
        SetupSimulationBuffer(width, height);
    }
}

void ImGuiContext::BeginSimulationRendering(const float clear_color[4]) {
    // Nothing needed here now - rendering happens in PrepareSimulationViewport
}

void ImGuiContext::EndSimulationRendering() {
    // Nothing needed here
}

bool ImGuiContext::ShowSimulationWindow() {
    // Use our simplified approach for direct rendering
    bool open = BeginSimulationViewWindow();

    if (open) {
        // Render directly to the window
        PrepareSimulationViewport(nullptr);

        // Add some debug text
        ImGui::SetCursorPos(ImVec2(10, 10));
        ImGui::TextColored(ImVec4(1, 1, 0, 1), "DIRECT RENDERING at Time: %.1f",
                           static_cast<float>(glfwGetTime()));
    }

    EndWindow();
    return open;
}

}  // namespace cshorelark