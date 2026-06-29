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
