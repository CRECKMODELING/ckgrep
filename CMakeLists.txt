cmake_minimum_required(VERSION 3.20)
project(
  ckgrep
  VERSION 0.1.0
  LANGUAGES CXX
  DESCRIPTION "A grep-like tool for searching CHEMKIN reaction mechanisms"
)

# Default to Release if no build type is set
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Setting build type to 'Release' as none was specified")
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build" FORCE)
endif()

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ckgrep uses <format> and <filesystem>, which require a reasonably recent
# standard library implementation.
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 13)
  message(FATAL_ERROR "ckgrep requires GCC >= 13 for <format> support (found ${CMAKE_CXX_COMPILER_VERSION})")
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 17)
  message(FATAL_ERROR "ckgrep requires Clang >= 17 for <format> support (found ${CMAKE_CXX_COMPILER_VERSION})")
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 15)
  message(FATAL_ERROR "ckgrep requires AppleClang >= 15 (Xcode 15+) for <format> support (found ${CMAKE_CXX_COMPILER_VERSION})")
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 19.29)
  message(FATAL_ERROR "ckgrep requires MSVC >= 19.29 (VS 2019 16.10+) for <format> support (found ${CMAKE_CXX_COMPILER_VERSION})")
endif()

include(cmake/CompilerOptions.cmake)

# External dependency
include(cmake/external/argparse.cmake)

add_subdirectory(source)
