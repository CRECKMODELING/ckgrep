# -----------------------------------------------------------------------------
# Compiler flags - collected into an INTERFACE target so source targets just
# link against it instead of duplicating flag logic.
#
# Usage: target_link_libraries(<target> PRIVATE ckgrep_compiler_options)
# -----------------------------------------------------------------------------
add_library(ckgrep_compiler_options INTERFACE)

target_compile_features(ckgrep_compiler_options INTERFACE cxx_std_20)

target_compile_options(ckgrep_compiler_options INTERFACE
  $<$<CXX_COMPILER_ID:GNU,Clang,AppleClang>:-Wall -Wextra -Wpedantic>
  $<$<CXX_COMPILER_ID:MSVC>:/W4>
)

# -----------------------------------------------------------------------------
# Code coverage (local, Clang/AppleClang only) - enable with
# -Dckgrep_ENABLE_COVERAGE=ON, then build and run the `coverage` target.
# -----------------------------------------------------------------------------
option(ckgrep_ENABLE_COVERAGE "Build with source-based coverage instrumentation (Clang only)" OFF)
if(ckgrep_ENABLE_COVERAGE)
  if(NOT CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    message(FATAL_ERROR "ckgrep_ENABLE_COVERAGE requires Clang or AppleClang (found ${CMAKE_CXX_COMPILER_ID})")
  endif()
  target_compile_options(ckgrep_compiler_options INTERFACE -fprofile-instr-generate -fcoverage-mapping)
  target_link_options(ckgrep_compiler_options INTERFACE -fprofile-instr-generate -fcoverage-mapping)
endif()
