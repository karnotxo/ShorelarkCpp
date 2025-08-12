#ifndef CSHORELARK_OPTIMIZER_CLI_CLI_ARGS_H
#define CSHORELARK_OPTIMIZER_CLI_CLI_ARGS_H

#include <filesystem>
#include <optional>
#include <string>
#include <tl/expected.hpp>
#include <variant>

#include "common.h"
#include "constants.h"

namespace cshorelark::optimizer_cli {

/**
 * @brief Command line arguments for the analyze command
 */
struct analyze_args {
    std::filesystem::path input_path;   ///< Path to input file with optimization data
    std::filesystem::path output_path;  ///< Path to save analysis output (optional)
};

/**
 * @brief Command line arguments for the simulate command
 */
struct simulate_args {
    std::filesystem::path output_path;  ///< Path to save output to
    std::size_t iterations =
        cshorelark::optimizer_cli::constants::k_default_iterations;  ///< Number of generations to
                                                                     ///< simulate
    std::size_t generations =
        cshorelark::optimizer_cli::constants::k_default_generations;  ///< Number of generations
                                                                      ///< to simulate
};

/**
 * @brief Command line arguments for the optimizer CLI
 *
 * This structure matches the command-based structure in the Rust implementation
 */
struct cli_args {
    enum class command_type { analyze, simulate };

    command_type cmd;                                ///< Which command to execute
    std::variant<analyze_args, simulate_args> args;  ///< Arguments for the selected command
};

/**
 * @brief Parse command line arguments
 *
 * @param argc Argument count
 * @param argv Argument values
 * @return Expected containing parsed arguments or error message
 */
auto parse_args(int argc, char* argv[]) -> tl::expected<cli_args, std::string>;

}  // namespace cshorelark::optimizer_cli

#endif  // CSHORELARK_OPTIMIZER_CLI_CLI_ARGS_H
