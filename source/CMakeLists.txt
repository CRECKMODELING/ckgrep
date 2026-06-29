add_executable(
  ckgrep
  pattern.cpp
  query.cpp
  matcher.cpp
  reaction_scanner.cpp
  cli.cpp
  utils.cpp
  main.cpp
)
target_link_libraries(ckgrep PRIVATE argparse ckgrep_compiler_options)
target_compile_definitions(ckgrep PRIVATE CKGREP_VERSION="${PROJECT_VERSION}")

include(GNUInstallDirs)
install(TARGETS ckgrep RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
