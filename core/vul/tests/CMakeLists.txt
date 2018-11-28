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
