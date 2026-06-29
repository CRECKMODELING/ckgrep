# -----------------------------------------------------------------------------
# argparse - header-only input argument parser
# https://github.com/p-ranav/argparse
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
