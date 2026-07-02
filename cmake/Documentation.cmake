# -----------------------------------------------------------------------------
# 'docs' target - Doxygen LaTeX/PDF reference manual (user guide + API docs).
# Only included when BUILD_DOC is ON; additionally requires Doxygen, a
# pdflatex toolchain, and make, otherwise the target is skipped with a notice.
# The target is not part of 'all': build it explicitly with
#   cmake --build <build dir> --target docs
# and find the result at <build dir>/docs/latex/refman.pdf.
# -----------------------------------------------------------------------------
find_package(Doxygen QUIET)
find_package(LATEX QUIET COMPONENTS PDFLATEX MAKEINDEX)
find_program(MAKE_EXECUTABLE NAMES make gmake)

if(NOT DOXYGEN_FOUND OR NOT LATEX_FOUND OR NOT MAKE_EXECUTABLE)
  message(STATUS "Doxygen/LaTeX/make not all available: 'docs' target disabled")
  return()
endif()

configure_file(
  ${PROJECT_SOURCE_DIR}/docs/Doxyfile.in
  ${CMAKE_BINARY_DIR}/docs/Doxyfile
  @ONLY
)

add_custom_target(
  docs
  COMMAND Doxygen::doxygen ${CMAKE_BINARY_DIR}/docs/Doxyfile
  COMMAND ${MAKE_EXECUTABLE} -C ${CMAKE_BINARY_DIR}/docs/latex
  WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
  COMMENT "Building documentation: ${CMAKE_BINARY_DIR}/docs/latex/refman.pdf"
  VERBATIM
)

# Ship the manual alongside the LICENSE (see source/CMakeLists.txt) when it
# has been built; OPTIONAL keeps `cmake --install` working when the 'docs'
# target was never run.
include(GNUInstallDirs)
install(
  FILES ${CMAKE_BINARY_DIR}/docs/latex/refman.pdf
  DESTINATION ${CMAKE_INSTALL_DOCDIR}
  RENAME ckgrep-manual.pdf
  OPTIONAL
)
