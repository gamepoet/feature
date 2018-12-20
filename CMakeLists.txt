cmake_minimum_required(VERSION 3.11)
project(feature LANGUAGES C CXX)

option(FEATURE_BUILD_TESTS "Build tests" OFF)
option(FEATURE_COVERAGE "Enable code coverage" OFF)

# max out the warning settings for the compilers (why isn't there a generic way to do this?)
if (MSVC)
  string(REPLACE "/W3" "" CMAKE_C_FLAGS ${CMAKE_C_FLAGS})
  string(REPLACE "/W3" "" CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})
endif()

add_library(
  feature
  STATIC
  src/feature.c
  src/feature.h
)
target_include_directories(
  feature
  PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
  $<INSTALL_INTERFACE:>
)
target_compile_options(
  feature
  PRIVATE
  $<$<CXX_COMPILER_ID:AppleClang>:-Wall -Wextra -Wpedantic -Wno-unused-parameter>
  $<$<CXX_COMPILER_ID:MSVC>:/W4 /wd4100>
)
if (FEATURE_COVERAGE)
  target_compile_options(feature PRIVATE $<$<CXX_COMPILER_ID:AppleClang>:--coverage>)
  if (CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")
    set_target_properties(feature PROPERTIES LINK_FLAGS --coverage)
  endif()
endif()

# test app
if (FEATURE_BUILD_TESTS)
  include(FetchContent)
  set(FETCHCONTENT_QUIET OFF)
  FetchContent_Declare(
    catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2
    GIT_TAG v2.5.0
  )
  FetchContent_GetProperties(catch2)
  if (NOT catch2_POPULATED)
    FetchContent_Populate(catch2)
  endif()

  add_executable(
    test_runner
    spec/main.cpp
    spec/feature_spec.cpp
  )
  target_include_directories(test_runner PRIVATE ${catch2_SOURCE_DIR}/single_include/catch2)
  target_compile_features(test_runner PRIVATE cxx_std_11)
  target_link_libraries(test_runner feature)
  target_compile_options(
    test_runner
    PRIVATE
    $<$<CXX_COMPILER_ID:AppleClang>:-Wall -Wextra -Wpedantic -Wno-unused-parameter>
    $<$<CXX_COMPILER_ID:MSVC>:/W4 /wd4100>
  )
  if (FEATURE_COVERAGE)
    target_compile_options(test_runner PRIVATE $<$<CXX_COMPILER_ID:AppleClang>:--coverage>)
    if (CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")
      set_target_properties(test_runner PROPERTIES LINK_FLAGS --coverage)
    endif()
  endif()

  # test suite
  enable_testing()
  add_test(NAME spec COMMAND test_runner)
endif()
