cc_binary = native.cc_binary
sh_binary = native.sh_binary
def benchmark_suite(
        name,
        src,
        deps,
        visibility = ["//main:__pkg__"],
):
    # C++ benchmark binary
    cc_binary(
        name = "benchmark_" + name,
        srcs = [src],
        deps = deps + [
            "@google_benchmark//:benchmark",
            "@google_benchmark//:benchmark_main",
        ],
        linkopts = ["-pthread"],
        copts = ["-O2", "-g"],
        visibility = visibility,
    )

    # Analysis runner
    sh_binary(
        name = "run_analysis_" + name,
        srcs = ["//utils:run_benchmark_sh"],
        data = [
            ":benchmark_" + name,
            "//utils:draw_py",
        ],
        args = ["$(location :benchmark_" + name + ")"],
    )

    # Perf runner
    sh_binary(
        name = "run_perf_" + name,
        srcs = ["//utils:run_perf_sh"],
        data = [
            ":benchmark_" + name,
        ],
        args = ["$(location :benchmark_" + name + ")"],
    )
