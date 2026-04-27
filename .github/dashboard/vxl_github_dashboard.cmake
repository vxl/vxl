#---------------------------------------------------------------------
# VXL CTest dashboard script for GitHub Actions
#---------------------------------------------------------------------
#
# Driven by environment variables set in .github/workflows/build.yml.
# Performs configure → build → test → submit-to-CDash in one ctest -S
# invocation. Output is kept minimal: build/test diagnostics live in
# CTest's XML files (which CDash consumes); only the summary lines
# reach the GitHub Actions log. The companion report_build_diagnostics.py
# script extracts the warning/error text from Build.xml after the run.
#
# Required environment variables:
#   CTEST_SOURCE_DIRECTORY  - source checkout root
#   CTEST_BINARY_DIRECTORY  - out-of-source build dir
#   CTEST_BUILD_NAME        - distinct CDash build identifier
#   CTEST_SITE              - CDash site label (usually github-actions)
#   DASHBOARD_MODEL         - Experimental | Continuous | Nightly
#   DASHBOARD_CACHE         - newline-separated CMake cache entries
#
# Optional:
#   CTEST_BUILD_CONFIGURATION   - default Release
#   CTEST_CMAKE_GENERATOR       - default Ninja
#   CTEST_BUILD_FLAGS           - default empty (e.g. "-j4")
#   CTEST_TEST_ARGS             - default "--output-on-failure --schedule-random"
#   DASHBOARD_DO_SUBMIT         - default 1 (set to 0 to skip ctest_submit)
#---------------------------------------------------------------------

cmake_minimum_required(VERSION 3.16)

# ----- Required env vars ---------------------------------------------
foreach(_var IN ITEMS
    CTEST_SOURCE_DIRECTORY
    CTEST_BINARY_DIRECTORY
    CTEST_BUILD_NAME
    CTEST_SITE
    DASHBOARD_MODEL
)
  if(NOT DEFINED ENV{${_var}} OR "$ENV{${_var}}" STREQUAL "")
    message(FATAL_ERROR "Required environment variable ${_var} is not set.")
  endif()
  set(${_var} "$ENV{${_var}}")
endforeach()

# ----- Optional env vars with defaults -------------------------------
if(DEFINED ENV{CTEST_BUILD_CONFIGURATION} AND NOT "$ENV{CTEST_BUILD_CONFIGURATION}" STREQUAL "")
  set(CTEST_BUILD_CONFIGURATION "$ENV{CTEST_BUILD_CONFIGURATION}")
else()
  set(CTEST_BUILD_CONFIGURATION "Release")
endif()
set(CTEST_CONFIGURATION_TYPE "${CTEST_BUILD_CONFIGURATION}")

if(DEFINED ENV{CTEST_CMAKE_GENERATOR} AND NOT "$ENV{CTEST_CMAKE_GENERATOR}" STREQUAL "")
  set(CTEST_CMAKE_GENERATOR "$ENV{CTEST_CMAKE_GENERATOR}")
else()
  set(CTEST_CMAKE_GENERATOR "Ninja")
endif()

if(DEFINED ENV{CTEST_BUILD_FLAGS})
  set(CTEST_BUILD_FLAGS "$ENV{CTEST_BUILD_FLAGS}")
endif()

if(DEFINED ENV{DASHBOARD_DO_SUBMIT} AND "$ENV{DASHBOARD_DO_SUBMIT}" STREQUAL "0")
  set(_do_submit FALSE)
else()
  set(_do_submit TRUE)
endif()

# ----- Initial cache --------------------------------------------------
# DASHBOARD_CACHE is one cache entry per line. Pass through verbatim.
set(CTEST_INITIAL_CACHE "$ENV{DASHBOARD_CACHE}")
file(WRITE "${CTEST_BINARY_DIRECTORY}/CMakeCache.txt" "${CTEST_INITIAL_CACHE}")

# ----- Run dashboard --------------------------------------------------
ctest_start("${DASHBOARD_MODEL}")

ctest_configure(
  BUILD   "${CTEST_BINARY_DIRECTORY}"
  SOURCE  "${CTEST_SOURCE_DIRECTORY}"
  RETURN_VALUE _configure_rc
  CAPTURE_CMAKE_ERROR _configure_capture
)

if(_configure_rc EQUAL 0 AND _configure_capture EQUAL 0)
  ctest_build(
    BUILD             "${CTEST_BINARY_DIRECTORY}"
    CONFIGURATION     "${CTEST_BUILD_CONFIGURATION}"
    NUMBER_WARNINGS   _build_warnings
    NUMBER_ERRORS     _build_errors
    RETURN_VALUE      _build_rc
    CAPTURE_CMAKE_ERROR _build_capture
  )
  message(STATUS "ctest_build: errors=${_build_errors} warnings=${_build_warnings} rc=${_build_rc}")

  if(_build_errors EQUAL 0 AND _build_rc EQUAL 0)
    ctest_test(
      BUILD               "${CTEST_BINARY_DIRECTORY}"
      RETURN_VALUE        _test_rc
      CAPTURE_CMAKE_ERROR _test_capture
      PARALLEL_LEVEL      $ENV{CTEST_PARALLEL_LEVEL}
    )
    message(STATUS "ctest_test: rc=${_test_rc}")
  endif()
endif()

if(_do_submit)
  ctest_submit(RETURN_VALUE _submit_rc CAPTURE_CMAKE_ERROR _submit_capture)
  message(STATUS "ctest_submit: rc=${_submit_rc}")
endif()

# ----- Final exit status ---------------------------------------------
# Hard-fail on configure or build errors so the GitHub job is red.
# Test failures are reported via CDash; the workflow does not turn red
# on a flaky test (matching ITK's CI behavior).
if(NOT _configure_rc EQUAL 0 OR NOT _configure_capture EQUAL 0)
  message(FATAL_ERROR "Configure step failed (rc=${_configure_rc} capture=${_configure_capture}).")
endif()
if(DEFINED _build_errors AND NOT _build_errors EQUAL 0)
  message(FATAL_ERROR "Build reported ${_build_errors} errors.")
endif()
if(DEFINED _build_rc AND NOT _build_rc EQUAL 0)
  message(FATAL_ERROR "Build returned non-zero (rc=${_build_rc}).")
endif()
