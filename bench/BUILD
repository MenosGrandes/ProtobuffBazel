
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
benchmark_suite(
    name = "serdel",
    src = "benchmark_serdel.cc",
    deps = [
       "//serdel/msg/base:msg_base_lib",  # 
       "//serdel/msg/a:a_msg_lib",  # 
              "//serdel/msg/b:b_msg_lib",  # 
              "//serdel/serializer:serializer_lib",  # 

    ],
)
benchmark_suite(
    name = "snake",
    src = "benchmark_snake.cc",
    deps = [
       "//camel:libcamel",  # 

    ],
)
