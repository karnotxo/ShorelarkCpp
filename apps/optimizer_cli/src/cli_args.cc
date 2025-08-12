#include "cli_args.h"

#include <spdlog/spdlog.h>

#include <args.hxx>
#include <filesystem>
#include <string>
#include <tl/expected.hpp>

#include "constants.h"

namespace cshorelark::optimizer_cli {

auto parse_args(int argc, char* argv[]) -> tl::expected<cli_args, std::string> {
    args::ArgumentParser parser("Neural network optimizer CLI");
    parser.Prog(argv[0]);
    parser.ProglinePostfix("{analyze|simulate}");
    args::HelpFlag help(parser, "help", "Display this help message", {'h', "help"});

    // Define subcommands
    args::Command analyze_cmd(parser, "analyze", "Analyze optimization results");
    args::Command simulate_cmd(parser, "simulate",
                               "Run simulation for neural network optimization");

    // Arguments for analyze command
    args::ValueFlag<std::string> analyze_input_path(analyze_cmd, "input",
                                                    "Path to input file with optimization data",
                                                    {'i', "input"}, args::Options::Required);
    args::ValueFlag<std::string> analyze_output_path(
        analyze_cmd, "output", "Path to save analysis output", {'o', "output"});

    // Arguments for simulate command
    args::ValueFlag<std::string> simulate_output_path(
        simulate_cmd, "output", "Path to save output files", {'o', "output"}, "output.toml");

    args::ValueFlag<std::size_t> iterations(
        simulate_cmd, "iterations", "Number of generations to simulate", {'i', "iterations"},
        cshorelark::optimizer_cli::constants::k_default_iterations);

    args::ValueFlag<std::size_t> generations(
        simulate_cmd, "generations", "Number of generations to simulate", {'g', "generations"},
        cshorelark::optimizer_cli::constants::k_default_generations);

    try {
        parser.ParseCLI(argc, argv);
    } catch (const args::Help&) {
        return tl::make_unexpected(parser.Help());
    } catch (const args::ParseError& e) {
        return tl::make_unexpected(std::string("Failed to parse arguments: ") + e.what());
    } catch (const args::ValidationError& e) {
        return tl::make_unexpected(std::string("Invalid argument: ") + e.what());
    }

    // Check which command was specified
    if (analyze_cmd) {
        analyze_args args_data;
        args_data.input_path = std::filesystem::path(args::get(analyze_input_path));

        if (analyze_output_path) {
            args_data.output_path = std::filesystem::path(args::get(analyze_output_path));
        }

        return cli_args{cli_args::command_type::analyze, args_data};
    }
    if (simulate_cmd) {
        // Construct simulation args
        simulate_args args_data;
        args_data.output_path = std::filesystem::path(args::get(simulate_output_path));
        args_data.iterations = args::get(iterations);
        args_data.generations = args::get(generations);

        return cli_args{cli_args::command_type::simulate, args_data};
    }

    return tl::make_unexpected("Please specify a command: analyze or simulate\n" + parser.Help());
}

}  // namespace cshorelark::optimizer_cli
