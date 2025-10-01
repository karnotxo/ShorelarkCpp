optimizer_inc = include_directories('include')

optimizer_lib = library('optimizer',
    [
        'src/simulate.cc'
    ],
    include_directories : optimizer_inc,
    dependencies : [
        fmt_dep,
        spdlog_dep,
        ranges_v3_dep,
        span_lite_dep,
        tl_expected_dep
    ],
    cpp_args : [
        '-DOPTIMIZER_EXPORTS',
        get_option('cpp_eh') == 'none' ? '-fno-exceptions' : '-fexceptions'
    ],
    install : true
)

# Declare dependency for other parts of the project
optimizer_dep = declare_dependency(
    link_with : optimizer_lib,
    include_directories : optimizer_inc
)
