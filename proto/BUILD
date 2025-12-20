load("@rules_cc//cc:cc_binary.bzl", "cc_binary")
load("@rules_cc//cc:cc_library.bzl", "cc_library")


proto_library(
    name = "my_proto_proto",
    srcs =["my_proto.proto"],
)
cc_proto_library(
    name = "my_proto_cc_proto",
    deps = [":my_proto_proto"],
)

cc_library(
    name = "proto_lib",  # This is the C++ library that will be used in the binary
    hdrs = [":my_proto_proto"],  # Include the headers for the generated code
    srcs = [":my_proto_cc_proto"],  # Include the C++ source files for the generated code
    deps = [
        "@protobuf//:protobuf",  # Link the protobuf runtime
    ],
    visibility = ["//bench:__pkg__"], #the bench is name of FOLDER!
)