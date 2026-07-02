# -----------------------------------------------------------------------------
# argparse - header-only command line argument parser
# https://github.com/p-ranav/argparse
#
# Fetched at configure time; provides the `argparse` INTERFACE target that the
# ckgrep executable links against.
# -----------------------------------------------------------------------------
message(STATUS "Fetching argparse ...")
include(FetchContent)
FetchContent_Declare(
    argparse
    GIT_REPOSITORY https://github.com/p-ranav/argparse.git
    GIT_TAG v3.2
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(argparse)
message(STATUS "argparse successfully fetched.")
