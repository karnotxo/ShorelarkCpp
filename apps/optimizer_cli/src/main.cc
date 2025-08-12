#include <spdlog/common.h>
#include <spdlog/spdlog.h>

#include <iostream>

#include "analyze.h"
#include "cli_args.h"
#include "simulate.h"

int main(int argc, char* argv[]) {
    // Initialize logger
    spdlog::set_pattern("[%^%l%$] %v");
    spdlog::set_level(spdlog::level::info);

    // Parse command line arguments
    auto args_result = cshorelark::optimizer_cli::parse_args(argc, argv);
    if (!args_result) {
        std::cerr << args_result.error() << std::endl;
        return 1;
    }

    // Dispatch based on command
    const auto& args = *args_result;

    switch (args.cmd) {
        case cshorelark::optimizer_cli::cli_args::command_type::analyze: {
            // Handle analyze command
            const auto& analyze_args = std::get<cshorelark::optimizer_cli::analyze_args>(args.args);

            auto result = cshorelark::optimizer_cli::analyze::run_analysis(
                analyze_args.input_path, analyze_args.output_path);
            if (!result) {
                spdlog::error(result.error());
                return 1;
            }

            spdlog::info(result.value());
            break;
        }

        case cshorelark::optimizer_cli::cli_args::command_type::simulate: {
            // Handle simulate command
            const auto& simulate_args =
                std::get<cshorelark::optimizer_cli::simulate_args>(args.args);

            // Create and run simulation
            cshorelark::optimizer_cli::simulation_runner runner(
                simulate_args.iterations, simulate_args.generations, simulate_args.output_path);

            runner.run();

            break;
        }
    }

    return 0;
}