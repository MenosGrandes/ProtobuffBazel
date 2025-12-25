
load("//bzl_rules:benchmark_rules.bzl", "benchmark_suite")

benchmark_suite(
    name = "cord",
    src = "benchmark_cord.cc",
    deps = [
        "//cord/inc:libcord",
    ],
)

benchmark_suite(
    name = "gpb",
    src = "benchmark_gpb.cc",
    deps = [
        "//proto:proto_lib",
    ],
)
