# -----------------------------------------------------------------------------
# GoogleTest - C++ testing framework
# https://github.com/google/googletest
#
# Fetched at configure time; provides the `GTest::gtest_main` target that the
# test binary links against. Only pulled in when BUILD_TESTING is ON.
# -----------------------------------------------------------------------------
message(STATUS "Fetching googletest ...")
include(FetchContent)
# Keep gtest out of `cmake --install` artifacts
set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
# On MSVC, build gtest against the shared C runtime so it matches ckgrep's
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.15.2
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(googletest)
message(STATUS "googletest successfully fetched.")
