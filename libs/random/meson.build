random_inc = include_directories('include')

random_lib = library('random',
    [
        'src/random.cc'
    ],
    include_directories : random_inc,
    dependencies : [
        fmt_dep,
        spdlog_dep,
        date_dep,
        tl_expected_dep
    ],
    cpp_args : [
        '-DRANDOM_EXPORTS',
        get_option('cpp_eh') == 'none' ? '-fno-exceptions' : '-fexceptions'
    ],
    install : true
)

# Declare dependency for other parts of the project
random_dep = declare_dependency(
    link_with : random_lib,
    include_directories : random_inc
)

# Tests
if get_option('build_tests')
    random_test = executable('random_test',
        [
            'test/random_test.cc'
        ],
        dependencies : [
            random_dep,
            catch2_dep
        ]
    )

    test('random_tests',
        random_test,
        protocol : 'tap',
        timeout : 180
    )
endif
