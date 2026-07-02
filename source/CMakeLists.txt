# -----------------------------------------------------------------------------
# ckgrep_core - search/parsing logic, with no CLI or main() attached, so it
# can be linked into both the ckgrep executable and the test binary. The
# compiler flags arrive through ckgrep_compiler_options and, being linked
# PUBLIC, propagate to every target that links the core library.
# -----------------------------------------------------------------------------
add_library(
  ckgrep_core
  STATIC
  pattern.cpp
  query.cpp
  matcher.cpp
  reaction_parser.cpp
  reaction_scanner.cpp
  utils.cpp
)
target_include_directories(ckgrep_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(ckgrep_core PUBLIC ckgrep_compiler_options)

# -----------------------------------------------------------------------------
# Main executable
# -----------------------------------------------------------------------------
add_executable(ckgrep cli.cpp main.cpp)
target_link_libraries(ckgrep PRIVATE ckgrep_core argparse)
# Single source of truth for the version printed by `ckgrep --version`:
# the project() version in the top-level CMakeLists.txt
target_compile_definitions(ckgrep PRIVATE CKGREP_VERSION="${PROJECT_VERSION}")

# -----------------------------------------------------------------------------
# Installation - `cmake --install <build dir> [--prefix <path>]`
# The PDF manual, when built, is installed by cmake/Documentation.cmake.
# -----------------------------------------------------------------------------
include(GNUInstallDirs)
install(TARGETS ckgrep RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
install(FILES ${PROJECT_SOURCE_DIR}/LICENSE DESTINATION ${CMAKE_INSTALL_DOCDIR})
