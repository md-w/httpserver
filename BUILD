load("@rules_cc//cc:defs.bzl", "cc_binary", "cc_library")

cc_binary(
    name = "httpserver",
    srcs = glob(
        ["src/*.cpp"],
        exclude = ["src/gtest-all.cpp"],
    ),
    includes = ["inc"],
    deps = [
        "@com_md_w_scratch//utils:cpp-utils",
        # "@com_google_benchmark//:benchmark",
    ],
)