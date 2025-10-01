simulation_ui_sources = files(
    'src/main.cc',
    'src/app.cc',
    'src/simulation_window.cc',
    'src/simulation_config.cc',
    'src/imgui_context.cc',
    'src/ImFileDialog.cpp',
    'src/imgui_stdlib.cpp'
)

# ImGui bindings
imgui_bindings_sources = files(
    '../../bindings/imgui_impl_glfw.cpp',
    '../../bindings/imgui_impl_opengl3.cpp'
)

simulation_ui_inc = include_directories('src', '../../bindings')

# Additional dependencies needed for GUI
glfw_dep = dependency('glfw3')
glew_dep = dependency('glew')
transwarp_dep = dependency('transwarp')
tomlpp_dep = dependency('tomlplusplus')
stb_dep = dependency('stb')
args_dep = dependency('taywee-args')

simulation_ui = executable('simulation_ui',
    [simulation_ui_sources, imgui_bindings_sources],
    include_directories : [simulation_ui_inc],
    dependencies : [
        random_dep,
        simulation_dep,
        neural_network_dep,
        genetic_algorithm_dep,
        stb_dep,
        imgui_dep,
        glfw_dep,
        glew_dep,
        transwarp_dep,
        args_dep,
        fmt_dep,
        spdlog_dep,
        date_dep,
        ranges_v3_dep,
        span_lite_dep,
        tl_expected_dep,
        tomlpp_dep
    ],
    install : true
)

if get_option('build_tests')
    simulation_ui_test_sources = files(
        'test/simulation_config_test.cc'
    )

    simulation_ui_test = executable('simulation_ui_test',
        simulation_ui_test_sources,
        dependencies : [
            catch2_dep,
            simulation_dep,
            imgui_dep,
            glfw_dep,
            glew_dep,
            fmt_dep,
            spdlog_dep,
            transwarp_dep,
            tomlpp_dep
        ],
        cpp_args : get_option('buildtype').startswith('debug') ? ['-fsanitize=address,undefined'] : [],
        link_args : get_option('buildtype').startswith('debug') ? ['-fsanitize=address,undefined'] : []
    )

    test('simulation_ui tests', simulation_ui_test)
endif 