# -----------------------------------------------------------------------------
# Compiler support check - ckgrep uses <format> and <filesystem>, which require
# a reasonably recent standard library implementation. Failing here at
# configure time beats a cryptic compile error halfway through the build.
# -----------------------------------------------------------------------------
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 13)
  message(FATAL_ERROR "ckgrep requires GCC >= 13 for <format> support (found ${CMAKE_CXX_COMPILER_VERSION})")
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 17)
  message(FATAL_ERROR "ckgrep requires Clang >= 17 for <format> support (found ${CMAKE_CXX_COMPILER_VERSION})")
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 15)
  message(FATAL_ERROR "ckgrep requires AppleClang >= 15 (Xcode 15+) for <format> support (found ${CMAKE_CXX_COMPILER_VERSION})")
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 19.29)
  message(FATAL_ERROR "ckgrep requires MSVC >= 19.29 (VS 2019 16.10+) for <format> support (found ${CMAKE_CXX_COMPILER_VERSION})")
endif()

# -----------------------------------------------------------------------------
# Compiler flags - collected into an INTERFACE target so source targets just
# link against it instead of duplicating flag logic. ckgrep_core links it
# PUBLIC, so everything downstream of the core library inherits the flags.
#
# Usage: target_link_libraries(<target> PRIVATE|PUBLIC ckgrep_compiler_options)
# -----------------------------------------------------------------------------
add_library(ckgrep_compiler_options INTERFACE)

target_compile_features(ckgrep_compiler_options INTERFACE cxx_std_20)

target_compile_options(ckgrep_compiler_options INTERFACE
  $<$<CXX_COMPILER_ID:GNU,Clang,AppleClang>:-Wall -Wextra -Wpedantic>
  $<$<CXX_COMPILER_ID:MSVC>:/W4>
)

# -----------------------------------------------------------------------------
# Code coverage (local, Clang/AppleClang only) - enable with
# -DENABLE_COVERAGE=ON, then build and run the `coverage` target
# (defined in tests/CMakeLists.txt). Only the instrumentation flags live here.
# -----------------------------------------------------------------------------
option(ENABLE_COVERAGE "Build with source-based coverage instrumentation (Clang only)" OFF)
if(ENABLE_COVERAGE)
  if(NOT CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    message(FATAL_ERROR "ENABLE_COVERAGE requires Clang or AppleClang (found ${CMAKE_CXX_COMPILER_ID})")
  endif()
  target_compile_options(ckgrep_compiler_options INTERFACE -fprofile-instr-generate -fcoverage-mapping)
  target_link_options(ckgrep_compiler_options INTERFACE -fprofile-instr-generate -fcoverage-mapping)
endif()
