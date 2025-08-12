#include "app.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <spdlog/spdlog.h>

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <utility>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace cshorelark {
namespace {

void GlfwErrorCallback(int error, const char* description) {
    spdlog::error("GLFW Error {}: {}", error, description);
}

constexpr const char* kGlslVersion = "#version 130";

// Path to the icons directory
const std::filesystem::path kIconsPath = "apps/simulation_ui/assets/icons";
const std::filesystem::path kIconsPathAlt = "assets/icons";

// Icon files to load, in order of preference
const std::vector<std::string> kIconFiles = {"app_icon16.png", "app_icon32.png", "app_icon48.png",
                                             "app_icon64.png", "app_icon128.png"};

}  // namespace

App::App(std::string_view title, int width, int height)
    : title_(title), width_(width), height_(height) {
    spdlog::debug("App constructor: initializing with title='{}', width={}, height={}", title,
                  width, height);

    try {
        InitGlfw();
        InitGlew();

        // Set window icon
        SetWindowIcon();

        spdlog::debug("Initializing ImGui context");
        imgui_context_ = std::make_unique<ImGuiContext>(window_, kGlslVersion);
        spdlog::debug("ImGui context initialized successfully");
    } catch (const std::exception& e) {
        spdlog::critical("Failed to initialize App: {}", e.what());
        throw;
    }

    spdlog::debug("App constructor completed successfully");
}

App::~App() {
    spdlog::debug("App destructor: cleaning up resources");

    imgui_context_.reset();
    spdlog::debug("ImGui context destroyed");

    if (window_) {
        glfwDestroyWindow(window_);
        spdlog::debug("GLFW window destroyed");
    }

    glfwTerminate();
    spdlog::debug("GLFW terminated");
}

App::App(App&& other) noexcept
    : window_(std::exchange(other.window_, nullptr)),
      title_(std::move(other.title_)),
      width_(other.width_),
      height_(other.height_),
      imgui_context_(std::move(other.imgui_context_)) {
    spdlog::debug("App move constructor");
}

App& App::operator=(App&& other) noexcept {
    spdlog::debug("App move assignment operator");

    if (this != &other) {
        imgui_context_.reset();
        if (window_) {
            glfwDestroyWindow(window_);
        }

        window_ = std::exchange(other.window_, nullptr);
        title_ = std::move(other.title_);
        width_ = other.width_;
        height_ = other.height_;
        imgui_context_ = std::move(other.imgui_context_);
    }
    return *this;
}

bool App::ShouldClose() const noexcept {
    bool should_close = glfwWindowShouldClose(window_);
    if (should_close) {
        spdlog::debug("Window close detected");
    }
    return should_close;
}

void App::RunFrame(const std::function<void()>& render_callback) {
    spdlog::trace("RunFrame: starting new frame");

    // Poll events
    glfwPollEvents();

    // Start the Dear ImGui frame
    spdlog::trace("RunFrame: starting ImGui frame");
    imgui_context_->new_frame();

    // Clear the background
    glClearColor(0.45f, 0.55f, 0.60f, 1.00f);
    glClear(GL_COLOR_BUFFER_BIT);

    // Execute the render callback if provided
    if (render_callback) {
        spdlog::trace("RunFrame: executing render callback");
        render_callback();
    }

    // Render ImGui
    spdlog::trace("RunFrame: rendering ImGui");
    imgui_context_->render();

    spdlog::trace("RunFrame: swapping buffers");
    glfwSwapBuffers(window_);

    spdlog::trace("RunFrame: frame completed");
}

void App::InitGlfw() {
    spdlog::debug("Initializing GLFW");

    glfwSetErrorCallback(GlfwErrorCallback);
    spdlog::debug("GLFW error callback set");

    if (!glfwInit()) {
        spdlog::critical("Failed to initialize GLFW");
        throw std::runtime_error("Failed to initialize GLFW");
    }
    spdlog::debug("GLFW initialized successfully");

    // GL 3.3 + GLSL 130
    spdlog::debug("Setting GLFW window hints: OpenGL 3.3 Core Profile");
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    // Create window with graphics context
    spdlog::debug("Creating GLFW window: {}x{}", width_, height_);
    window_ = glfwCreateWindow(width_, height_, title_.c_str(), nullptr, nullptr);
    if (!window_) {
        glfwTerminate();
        spdlog::critical("Failed to create GLFW window");
        throw std::runtime_error("Failed to create GLFW window");
    }
    spdlog::debug("GLFW window created successfully");

    glfwMakeContextCurrent(window_);
    glfwSwapInterval(1);  // Enable vsync
    spdlog::debug("OpenGL context made current, vsync enabled");
}

void App::InitGlew() {
    spdlog::debug("Initializing GLEW");

    GLenum glew_result = glewInit();
    if (glew_result != GLEW_OK) {
        spdlog::critical("Failed to initialize GLEW: {}",
                         reinterpret_cast<const char*>(glewGetErrorString(glew_result)));
        throw std::runtime_error("Failed to initialize GLEW");
    }

    spdlog::debug("GLEW initialized successfully");
    spdlog::info("OpenGL version: {}", reinterpret_cast<const char*>(glGetString(GL_VERSION)));
    spdlog::info("GLEW version: {}", reinterpret_cast<const char*>(glewGetString(GLEW_VERSION)));
    spdlog::info("GPU vendor: {}", reinterpret_cast<const char*>(glGetString(GL_VENDOR)));
    spdlog::info("GPU renderer: {}", reinterpret_cast<const char*>(glGetString(GL_RENDERER)));
}

void App::SetWindowIcon() {
    spdlog::debug("Setting window icon from assets");

    std::vector<GLFWimage> images;

    // Try to load each icon file from the source directory path first
    for (const auto& icon_file : kIconFiles) {
        std::filesystem::path icon_path = kIconsPath / icon_file;
        spdlog::debug("Trying to load icon: {}", icon_path.string());

        // Load the image using stb_image
        int width, height, channels;
        unsigned char* pixels =
            stbi_load(icon_path.string().c_str(), &width, &height, &channels, 4);

        // If loading failed, try the alternative build directory path
        if (!pixels) {
            icon_path = kIconsPathAlt / icon_file;
            spdlog::debug("First path failed, trying alternative path: {}", icon_path.string());
            pixels = stbi_load(icon_path.string().c_str(), &width, &height, &channels, 4);
        }

        if (pixels) {
            spdlog::debug("Successfully loaded icon {}x{} with {} channels", width, height,
                          channels);

            // Store the loaded data
            IconData icon_data;
            icon_data.width = width;
            icon_data.height = height;
            icon_data.pixels.resize(width * height * 4);
            std::copy(pixels, pixels + (width * height * 4), icon_data.pixels.begin());

            // Free the stb_image data
            stbi_image_free(pixels);

            // Add to icon data collection
            icon_data_.push_back(std::move(icon_data));

            // Create GLFW image
            GLFWimage image;
            image.width = icon_data_.back().width;
            image.height = icon_data_.back().height;
            image.pixels = icon_data_.back().pixels.data();
            images.push_back(image);
        } else {
            spdlog::warn("Failed to load icon from both paths: {}", icon_file);
        }
    }

    if (!images.empty()) {
        // Set the window icons
        glfwSetWindowIcon(window_, static_cast<int>(images.size()), images.data());
        spdlog::info("Set {} window icons", images.size());
    } else {
        spdlog::warn("No window icons could be loaded, using default");
    }
}

}  // namespace cshorelark