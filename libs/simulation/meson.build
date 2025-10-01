simulation_sources = files(
    'src/statistics.cc',
    'src/animal_individual.cc',
    'src/brain.cc',
    'src/animal.cc',
    'src/eye.cc',
    'src/food.cc',
    'src/world.cc',
    'src/simulation.cc',
    'src/simulation_error.cc'
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
        'test/config_test.cc',
        'test/statistics_test.cc',
        'test/animal_individual_test.cc',
        'test/animal_test.cc',
        'test/brain_test.cc',
        'test/eye_test.cc',
        'test/food_test.cc',
        'test/vector2d_test.cc',
        'test/world_test.cc'
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