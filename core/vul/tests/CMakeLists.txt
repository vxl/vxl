# This is core/vul/tests/CMakeLists.txt

ADD_EXECUTABLE( vul_test_all
  test_driver.cxx

  test_arg.cxx
  test_file_iterator.cxx
  test_psfile.cxx
  test_sprintf.cxx
  test_url.cxx
  test_temp_filename.cxx
  test_regexp.cxx
  test_string.cxx
)
TARGET_LINK_LIBRARIES( vul_test_all vul vpl testlib vcl )

ADD_TEST( vul_test_string         vul_test_all test_string )
ADD_TEST( vul_test_arg            vul_test_all test_arg )
ADD_TEST( vul_test_file_iterator  vul_test_all test_file_iterator )
ADD_TEST( vul_test_psfile         vul_test_all test_psfile )
ADD_TEST( vul_test_sprintf        vul_test_all test_sprintf )
ADD_TEST( vul_test_url            vul_test_all test_url )
ADD_TEST( vul_test_temp_filename  vul_test_all test_temp_filename )
ADD_TEST( vul_test_regexp         vul_test_all test_regexp )

ADD_EXECUTABLE( vul_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( vul_test_include vul )
