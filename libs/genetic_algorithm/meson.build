genetic_algorithm_inc = include_directories('include')

genetic_algorithm_lib = library('genetic_algorithm',
    [
        'src/genetic_algorithm.cc',
        'src/chromosome.cc',
        'src/individual.cc',
        'src/selection.cc',
        'src/mutation.cc',
        'src/crossover.cc'
    ],
    include_directories : genetic_algorithm_inc,
    dependencies : [
        fmt_dep,
        spdlog_dep,
        ranges_v3_dep,
        span_lite_dep,
        tl_expected_dep,
        tl_optional_dep
    ],
    cpp_args : [
        '-DGENETIC_ALGORITHM_EXPORTS',
        get_option('cpp_eh') == 'none' ? '-fno-exceptions' : '-fexceptions'
    ],
    install : true
)

# Declare dependency for other parts of the project
genetic_algorithm_dep = declare_dependency(
    link_with : genetic_algorithm_lib,
    include_directories : genetic_algorithm_inc
)

# Tests
if get_option('build_tests')
    genetic_algorithm_test = executable('genetic_algorithm_test',
        [
            'test/genetic_algorithm_test.cc',
            'test/chromosome_test.cc',
            'test/individual_test.cc',
            'test/selection_test.cc',
            'test/mutation_test.cc',
            'test/crossover_test.cc',
            'test/statistics_test.cc'
        ],
        dependencies : [
            genetic_algorithm_dep,
            catch2_dep
        ]
    )

    test('genetic_algorithm_tests',
        genetic_algorithm_test,
        protocol : 'tap',
        timeout : 180
    )
endif

# Documentation
if get_option('build_docs')
    doxygen = find_program('doxygen', required : false)
    if doxygen.found()
        doxyfile = configure_file(
            input : 'Doxyfile.in',
            output : 'Doxyfile',
            configuration : {
                'PROJECT_NAME' : '"Genetic Algorithm"',
                'PROJECT_VERSION' : meson.project_version(),
                'OUTPUT_DIRECTORY' : meson.current_build_dir() / 'docs',
                'INPUT' : meson.current_source_dir() / 'include ' + meson.current_source_dir() / 'README.md',
                'GENERATE_HTML' : 'YES',
                'GENERATE_XML' : 'YES'
            }
        )

        custom_target('genetic_algorithm_docs',
            input : doxyfile,
            output : 'docs',
            command : [doxygen, doxyfile],
            build_by_default : true
        )
    endif
endif 