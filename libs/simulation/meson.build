simulation_sources = files(
    'src/statistics.cpp',
    'src/animal_individual.cpp',
    'src/brain.cpp',
    'src/animal.cpp',
    'src/eye.cpp',
    'src/food.cpp',
    'src/world.cpp'
)

simulation_inc = include_directories('include')

simulation_lib = library('simulation',
    simulation_sources,
    include_directories : [simulation_inc, inc],
    dependencies : [
        fmt_dep,
        spdlog_dep,
        date_dep,
        ranges_v3_dep,
        span_lite_dep,
        tl_expected_dep,
        tl_optional_dep
    ],
    install : true
)

simulation_dep = declare_dependency(
    link_with : simulation_lib,
    include_directories : [simulation_inc, inc]
)

if get_option('build_tests')
    simulation_test_sources = files(
        'test/config_test.cpp',
        'test/statistics_test.cpp',
        'test/animal_individual_test.cpp'
    )

    simulation_test = executable('simulation_test',
        simulation_test_sources,
        dependencies : [
            simulation_dep,
            catch2_dep
        ],
        cpp_args : get_option('buildtype').startswith('debug') ? ['-fsanitize=address,undefined'] : [],
        link_args : get_option('buildtype').startswith('debug') ? ['-fsanitize=address,undefined'] : []
    )

    test('simulation tests', simulation_test)
endif 