# -----------------------------------------------------------------------------
# GoogleTest - C++ testing framework
# https://github.com/google/googletest
# -----------------------------------------------------------------------------
message(STATUS "Fetching googletest ...")
include(FetchContent)
set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.15.2
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(googletest)
message(STATUS "googletest successfully fetched.")
