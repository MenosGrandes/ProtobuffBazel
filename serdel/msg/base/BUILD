
cc_library(
    name = "msg_base_lib",
    srcs = glob(["*.cc"], allow_empty=True),
    hdrs = glob(["*.h"], allow_empty=True),
    visibility = ["//visibility:public"],  # needed for subpackages
        deps = [
    "//serdel/serializer:serializer_lib",  # 
    ]
)
