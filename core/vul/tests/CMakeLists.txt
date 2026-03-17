
#---------------------------------------------------------------------
# GENERATE_TEST_DRIVER(<lib> <sources> [<lib1> <lib2> ...])
#
# - lib     : name of library being tested (e.g., vil, vul, etc.)
# - sources : variable containing the list of source files
# - libN    : libraries to link to
#
# If a test needs to be passed some arguments, you can provide them in
# a cmake variable named by the tests filename appended with '_args'
# (e.g., test_arg_args).
#
# Example usage:
#   set(vil_test_sources
#     ...
#     test_stream.cxx
#     ...
#   )
#   set(test_stream_args ${CMAKE_CURRENT_SOURCE_DIR}/file_read_data)
#   GENERATE_TEST_DRIVER(vil vil_test_sources vil vpl vul testlib vcl)
#---------------------------------------------------------------------
macro(GENERATE_TEST_DRIVER LIB SOURCES)
  create_test_sourcelist(test_driver_sources ${LIB}_test_driver.cxx
    ${${SOURCES}}
  )

  add_executable(${LIB}_test_driver ${test_driver_sources})
  # ***** what if ARGN is empty?
  target_link_libraries(${LIB}_test_driver ${ARGN})

  set(tests_to_run ${test_driver_sources})
  list(REMOVE_ITEM tests_to_run ${LIB}_test_driver.cxx)

  foreach(test ${tests_to_run})
    get_filename_component(test_name ${test} NAME_WE)
    if( NOT "${test_name}" MATCHES ".*_test_driver.*" ) # Skip the driver name itself
      add_test( NAME ${LIB}_${test_name}
              COMMAND $<TARGET_FILE:${LIB}_test_driver> ${test_name} ${${test_name}_args}
              )
    endif()
  endforeach()
endmacro()

#---------------------------------------------------------------------
# GENERATE_TEST_INCLUDE(<lib> <sources> <prefix>)
#
# - lib     : name of library (e.g., vil, vil_io, pbl, etc.)
# - sources : variable containing the list of library sources
# - prefix  : prefix used in the include statement
#
# Example usage:
#   GENERATE_TEST_INCLUDE(vil_io vil_io_sources "vil/io/")
#---------------------------------------------------------------------
macro(GENERATE_TEST_INCLUDE LIB SOURCES PREFIX)
  set(CMAKE_CONFIGURABLE_FILE_CONTENT "/* */\n")
  foreach(FILE ${${SOURCES}})
    get_filename_component(FILE_EXT ${FILE} EXT)
    if(FILE_EXT STREQUAL ".h")
      set(CMAKE_CONFIGURABLE_FILE_CONTENT
          "${CMAKE_CONFIGURABLE_FILE_CONTENT}#include <${PREFIX}${FILE}>\n#include <${PREFIX}${FILE}>\n")
    endif()
  endforeach()

  set(CMAKE_CONFIGURABLE_FILE_CONTENT
      "${CMAKE_CONFIGURABLE_FILE_CONTENT}\n\nint main(){return 0;}\n")

  configure_file("${CMAKE_ROOT}/Modules/CMakeConfigurableFile.in"
                 "${CMAKE_CURRENT_BINARY_DIR}/test_include.cxx"
                 @ONLY)

  add_executable(${LIB}_test_include ${CMAKE_CURRENT_BINARY_DIR}/test_include.cxx)
  target_link_libraries(${LIB}_test_include ${LIB})
endmacro()
# This is core/vul/tests/CMakeLists.txt

set(vul_test_sources
  test_arg.cxx
  test_file_iterator.cxx
  test_vul_file.cxx
  test_ios_state.cxx
  test_psfile.cxx
  test_sprintf.cxx
  test_temp_filename.cxx
  test_regexp.cxx
  test_string.cxx
  test_sleep_timer.cxx
  test_get_time_as_string.cxx
  test_awk.cxx
  test_expand_path.cxx
  test_debug.cxx
  test_checksum.cxx
)

#if(NOT APPLE)
  set(vul_test_sources ${vul_test_sources}
    test_url.cxx
  )
#endif()

GENERATE_TEST_DRIVER(${VXL_LIB_PREFIX}vul vul_test_sources ${VXL_LIB_PREFIX}vul ${VXL_LIB_PREFIX}vpl ${VXL_LIB_PREFIX}testlib ${VXL_LIB_PREFIX}vcl)

set(vul_test_include_sources
  ${vul_sources}
  ${vul_network_sources}
)
GENERATE_TEST_INCLUDE(${VXL_LIB_PREFIX}vul vul_test_include_sources "vul/")

add_executable( vul_try_reg_exp try_reg_exp.cxx )
target_link_libraries( vul_try_reg_exp ${VXL_LIB_PREFIX}vul ${VXL_LIB_PREFIX}vpl )
