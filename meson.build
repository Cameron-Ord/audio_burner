project('burner', 'c')
src = [
        'src/main.c',
]

libburn_dep = dependency('libburn-1', required: true)
cmp_time_args = [
    '-Wall', '-Wextra', '-Wpedantic'
]

target = executable(
        meson.project_name(),
        src,
        dependencies: [libburn_dep],
        c_args: cmp_time_args
)
