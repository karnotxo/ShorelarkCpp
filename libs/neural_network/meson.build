neural_network_inc = include_directories('include')

neural_network_lib = library('neural_network',
    [
        'src/network.cc',
        'src/layer.cc'
    ],
    include_directories : neural_network_inc,
    dependencies : [
        fmt_dep,
        spdlog_dep,
        date_dep,
        ranges_v3_dep,
        span_lite_dep,
        tl_expected_dep,
        tl_optional_dep
    ],
    cpp_args : [
        '-DNEURAL_NETWORK_EXPORTS',
        get_option('cpp_eh') == 'none' ? '-fno-exceptions' : '-fexceptions'
    ],
    install : true
)

# Declare dependency for other parts of the project
neural_network_dep = declare_dependency(
    link_with : neural_network_lib,
    include_directories : neural_network_inc
)

# Tests
if get_option('build_tests')
    neural_network_test = executable('neural_network_test',
        [
            'test/neuron_test.cc',
            'test/layer_test.cc',
            'test/network_test.cc'
        ],
        dependencies : [
            neural_network_dep,
            catch2_dep
        ]
    )

    test('neural_network_tests',
        neural_network_test,
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
                'PROJECT_NAME' : '"Neural Network"',
                'PROJECT_VERSION' : meson.project_version(),
                'OUTPUT_DIRECTORY' : meson.current_build_dir() / 'docs',
                'INPUT' : meson.current_source_dir() / 'include ' + meson.current_source_dir() / 'README.md',
                'GENERATE_HTML' : 'YES',
                'GENERATE_XML' : 'YES'
            }
        )

        custom_target('neural_network_docs',
            input : doxyfile,
            output : 'docs',
            command : [doxygen, doxyfile],
            build_by_default : true
        )
    endif
endif 