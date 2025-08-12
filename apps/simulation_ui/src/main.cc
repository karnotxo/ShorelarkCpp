#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <spdlog/spdlog.h>

#include <args.hxx>
#include <iostream>
#include <stdexcept>
#include <string>

#include "app.h"
#include "simulation_window.h"

int main(int argc, char* argv[]) {
    // Setup command line argument parsing
    args::ArgumentParser parser("CShorelark Simulation", "Evolutionary neural network simulation");
    args::HelpFlag help(parser, "help", "Display this help menu", {'h', "help"});

    // Simulation configuration arguments
    args::ValueFlag<int> initialPopulation(parser, "population", "Initial population size",
                                           {'p', "population"}, 100);
    args::ValueFlag<float> mutationRate(parser, "mutation-rate",
                                        "Mutation rate for genetic algorithm",
                                        {'m', "mutation-rate"}, 0.01f);
    args::ValueFlag<int> hiddenNeurons(parser, "hidden", "Number of hidden neurons", {"hidden"},
                                       10);
    args::Flag debugMode(parser, "debug", "Enable debug mode with additional logging",
                         {'d', "debug"});
    args::Flag traceMode(parser, "trace", "Enable trace mode with extremely verbose logging",
                         {'t', "trace"});

    // Window settings
    args::ValueFlag<int> windowWidth(parser, "width", "Window width in pixels", {'w', "width"},
                                     1280);
    args::ValueFlag<int> windowHeight(parser, "height", "Window height in pixels", {"height"}, 720);
    args::ValueFlag<std::string> windowTitle(parser, "title", "Window title", {'t', "title"},
                                             "CShorelark Simulation");

    try {
        spdlog::info("Parsing command line arguments");
        parser.ParseCLI(argc, argv);
    } catch (const args::Help&) {
        std::cout << parser;
        return 0;
    } catch (const args::ParseError& e) {
        spdlog::error("Command line parse error: {}", e.what());
        std::cerr << e.what() << std::endl;
        std::cerr << parser;
        return 1;
    }

    // Configure logging level
    if (traceMode) {
        spdlog::set_level(spdlog::level::trace);
        spdlog::trace("Trace logging enabled - extremely verbose output");
    } else if (debugMode) {
        spdlog::set_level(spdlog::level::debug);
        spdlog::debug("Debug logging enabled");
    } else {
        spdlog::set_level(spdlog::level::info);
        spdlog::info("Info logging enabled by default");
    }

    try {
        spdlog::info("Starting application with window size {}x{}", args::get(windowWidth),
                     args::get(windowHeight));

        // Create and run app - App handles all GLFW/GLEW initialization
        spdlog::debug("Creating App instance");
        cshorelark::App app(args::get(windowTitle), args::get(windowWidth),
                            args::get(windowHeight));
        spdlog::info("App created successfully");

        // Create simulation window
        cshorelark::simulation_window sim_window;

        spdlog::info("Entering main application loop");
        int frame_count = 0;
        // Run the main application loop
        while (!app.ShouldClose()) {
            // Log every 100 frames to avoid flooding
            if (frame_count % 100 == 0) {
                spdlog::debug("Processing frame {}", frame_count);
            }

            // Run the frame with the simulation window render as a callback
            app.RunFrame([&sim_window]() { sim_window.render(); });

            frame_count++;
        }

        spdlog::info("Application loop ended after {} frames", frame_count);
        return 0;
    } catch (const std::exception& e) {
        spdlog::critical("Fatal error: {}", e.what());
        return 1;
    } catch (...) {
        spdlog::critical("Unknown fatal error occurred");
        return 1;
    }
}