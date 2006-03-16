# This is core/vul/tests/CMakeLists.txt

SET(vul_test_sources
  test_driver.cxx

  test_arg.cxx
  test_file_iterator.cxx
  test_vul_file.cxx
  test_psfile.cxx
  test_sprintf.cxx
  test_temp_filename.cxx
  test_regexp.cxx
  test_string.cxx
  test_sleep_timer.cxx
)


IF (APPLE)
  ADD_EXECUTABLE(vul_test_all ${vul_test_sources})
ELSE (APPLE)
  ADD_EXECUTABLE(vul_test_all
    ${vul_test_sources}
    test_url.cxx
)
ENDIF(APPLE)

TARGET_LINK_LIBRARIES( vul_test_all vul vpl testlib vcl )

ADD_TEST( vul_test_string         vul_test_all test_string )
ADD_TEST( vul_test_arg            vul_test_all test_arg )
ADD_TEST( vul_test_file_iterator  vul_test_all test_file_iterator )
ADD_TEST( vul_test_file           vul_test_all test_vul_file)
ADD_TEST( vul_test_sleep_timer    vul_test_all test_sleep_timer )
ADD_TEST( vul_test_psfile         vul_test_all test_psfile )
ADD_TEST( vul_test_sprintf        vul_test_all test_sprintf )
IF (NOT APPLE)
  ADD_TEST( vul_test_url            vul_test_all test_url )
ENDIF(NOT APPLE)
ADD_TEST( vul_test_temp_filename  vul_test_all test_temp_filename )
ADD_TEST( vul_test_regexp         vul_test_all test_regexp )

ADD_EXECUTABLE( vul_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( vul_test_include vul )
ADD_EXECUTABLE( vul_test_debug test_debug.cxx )
TARGET_LINK_LIBRARIES( vul_test_debug vul vpl testlib )
