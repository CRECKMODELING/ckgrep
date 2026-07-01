add_executable(
  ckgrep_tests
  test_string_utils.cpp
  test_query.cpp
  test_pattern.cpp
  test_matcher.cpp
  test_reaction_parser.cpp
  test_reaction_scanner.cpp
)
target_link_libraries(ckgrep_tests PRIVATE ckgrep_core GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(ckgrep_tests)

if(ckgrep_ENABLE_COVERAGE)
  find_program(LLVM_PROFDATA llvm-profdata)
  find_program(LLVM_COV llvm-cov)
  if((NOT LLVM_PROFDATA OR NOT LLVM_COV) AND APPLE)
    # On macOS these ship inside the Xcode/CLT toolchain rather than on PATH.
    execute_process(
      COMMAND xcrun --find llvm-profdata
      OUTPUT_VARIABLE LLVM_PROFDATA
      OUTPUT_STRIP_TRAILING_WHITESPACE ERROR_QUIET
    )
    execute_process(
      COMMAND xcrun --find llvm-cov
      OUTPUT_VARIABLE LLVM_COV
      OUTPUT_STRIP_TRAILING_WHITESPACE ERROR_QUIET
    )
  endif()
  if(NOT LLVM_PROFDATA OR NOT LLVM_COV)
    message(
      FATAL_ERROR
      "ckgrep_ENABLE_COVERAGE requires llvm-profdata and llvm-cov (install"
      " LLVM or Xcode Command Line Tools)"
    )
  endif()

  add_custom_target(
    coverage
    COMMAND ${CMAKE_COMMAND} -E env
            LLVM_PROFILE_FILE=${CMAKE_CURRENT_BINARY_DIR}/ckgrep.profraw
            $<TARGET_FILE:ckgrep_tests>
    COMMAND ${LLVM_PROFDATA} merge -sparse
            ${CMAKE_CURRENT_BINARY_DIR}/ckgrep.profraw
            -o ${CMAKE_CURRENT_BINARY_DIR}/ckgrep.profdata
    COMMAND ${LLVM_COV} report $<TARGET_FILE:ckgrep_tests>
            -instr-profile=${CMAKE_CURRENT_BINARY_DIR}/ckgrep.profdata
            -object $<TARGET_FILE:ckgrep_core>
            --ignore-filename-regex=_deps/
    COMMAND ${LLVM_COV} show $<TARGET_FILE:ckgrep_tests>
            -instr-profile=${CMAKE_CURRENT_BINARY_DIR}/ckgrep.profdata
            -object $<TARGET_FILE:ckgrep_core>
            --ignore-filename-regex=_deps/
            -format=html
            -output-dir=${CMAKE_CURRENT_BINARY_DIR}/coverage-html
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    DEPENDS ckgrep_tests
    COMMENT
      "Running tests with coverage and generating report at"
      " ${CMAKE_CURRENT_BINARY_DIR}/coverage-html/index.html"
    VERBATIM
  )
endif()
