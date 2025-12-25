load("@rules_cc//cc:cc_binary.bzl", "cc_binary")

cc_binary(
    name = "benchmark",
    srcs = ["benchmark.cc"],
    deps = [
     "//proto:proto_lib",#the proto is name of FOLDER!
     "//cord:libcord",#the proto is name of FOLDER!

    "@google_benchmark//:benchmark",
    "@google_benchmark//:benchmark_main",

    ],
    linkopts = ["-pthread"],
    visibility = ["//main:__pkg__"],
    copts = ["-O2", "-g"],  # keep optimizations, but include debug info

)
sh_binary(
    name = "run_analysis",
    srcs = ["//utils:run_benchmark_sh"],
    data = [ "//bench:benchmark","//utils:draw_py"],  # ensure the binary is available
    args = ["$(location //bench:benchmark)"],

)
sh_binary(
    name = "run_perf",
    srcs = ["//utils:run_perf_sh"],
    data = ["//bench:benchmark"],
    args = ["$(location //bench:benchmark)"],

)