optimizer_cli_sources = files(
    'src/main.cc',
    'src/analyze.cc',
    'src/cli_args.cc',
    'src/simulate.cc'
)

optimizer_cli_inc = include_directories('src')

# Additional dependencies needed for CLI
args_dep = dependency('taywee-args')
json_dep = dependency('nlohmann_json')
transwarp_dep = dependency('transwarp')

optimizer_cli = executable('optimizer_cli',
    optimizer_cli_sources,
    include_directories : [optimizer_cli_inc],
    dependencies : [
        genetic_algorithm_dep,
        neural_network_dep,
        random_dep,
        simulation_dep,
        fmt_dep,
        spdlog_dep,
        args_dep,
        span_lite_dep,
        tl_expected_dep,
        json_dep,
        transwarp_dep
    ],
    install : true
)

if get_option('build_tests')
    optimizer_cli_test_sources = files(
        'test/analyze_test.cc',
        'test/config_test.cc',
        'test/optimizer_test.cc',
        'test/simulate_test.cc'
    )

    optimizer_cli_test = executable('optimizer_cli_test',
        optimizer_cli_test_sources,
        dependencies : [
            catch2_dep,
            genetic_algorithm_dep,
            neural_network_dep,
            random_dep,
            simulation_dep,
            fmt_dep,
            spdlog_dep,
            args_dep,
            span_lite_dep,
            tl_expected_dep,
            json_dep,
            transwarp_dep
        ],
        cpp_args : get_option('buildtype').startswith('debug') ? ['-fsanitize=address,undefined'] : [],
        link_args : get_option('buildtype').startswith('debug') ? ['-fsanitize=address,undefined'] : []
    )

    test('optimizer_cli tests', optimizer_cli_test)
endif
