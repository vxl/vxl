# This is core/vul/tests/CMakeLists.txt

SET(vul_test_sources
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
)

IF (NOT APPLE)
  SET(vul_test_sources ${vul_test_sources}
    test_url.cxx
  )
ENDIF(NOT APPLE)

GENERATE_TEST_DRIVER(vul vul_test_sources vul vpl testlib vcl)

SET(vul_test_include_sources
  ${vul_sources}
  ${vul_network_sources}
)
GENERATE_TEST_INCLUDE(vul vul_test_include_sources "vul/")

ADD_EXECUTABLE( vul_test_debug test_debug.cxx )
TARGET_LINK_LIBRARIES( vul_test_debug vul vpl testlib )

ADD_EXECUTABLE( vul_try_reg_exp try_reg_exp.cxx )
TARGET_LINK_LIBRARIES( vul_try_reg_exp vul vpl )
