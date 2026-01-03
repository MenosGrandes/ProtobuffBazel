
cc_library(
    name = "a_msg_lib",
    srcs = glob(["*.cc"], allow_empty=True),
    hdrs = glob(["*.h"], allow_empty=True),
    visibility = ["//visibility:public"],  # needed for subpackages
        deps = [
        "//serdel/msg/base:msg_base_lib",  # 
            "//serdel/serializer:serializer_lib",  # 
            "//serdel/registry:registry_lib",  # 

        ]

)
