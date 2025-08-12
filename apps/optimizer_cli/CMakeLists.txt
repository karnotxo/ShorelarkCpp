cmake_minimum_required(VERSION 3.20)

add_executable(optimizer_cli
    src/main.cc
    src/analyze.cc
    src/cli_args.cc  
    src/simulate.cc
)

target_include_directories(optimizer_cli PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Add dependencies
target_link_libraries(optimizer_cli
    PRIVATE
        cshorelark::genetic_algorithm
        cshorelark::neural_network
        cshorelark::random
        cshorelark::simulation        
        fmt::fmt
        spdlog::spdlog
        taywee::args
        nonstd::span-lite
        tl::expected
        nlohmann_json::nlohmann_json
        transwarp::transwarp
)

# Set C++17 standard
target_compile_features(optimizer_cli PRIVATE cxx_std_17)

# Create alias target
add_executable(cshorelark::optimizer_cli ALIAS optimizer_cli)

# Install binary
install(TARGETS optimizer_cli
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)