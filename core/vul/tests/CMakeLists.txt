INCLUDE( ${allvxl_SOURCE_DIR}/vxl/testlib/CMakeListsLink.txt )
INCLUDE( ${allvxl_SOURCE_DIR}/vxl/vul/CMakeListsLink.txt )
INCLUDE( ${allvxl_SOURCE_DIR}/vxl/vpl/CMakeListsLink.txt )
INCLUDE( ${allvxl_SOURCE_DIR}/vcl/CMakeListsLink.txt )

ADD_EXECUTABLE( vul_test_driver
  test_driver.cxx

  test_include.cxx
  test_arg.cxx
  test_file_iterator.cxx
  test_psfile.cxx
  test_sprintf.cxx
  test_url.cxx
  test_temp_filename.cxx
)

ADD_TEST( vul_test_include        vul_test_driver test_include )
ADD_TEST( vul_test_arg            vul_test_driver test_arg )
ADD_TEST( vul_test_file_iterator  vul_test_driver test_file_iterator )
ADD_TEST( vul_test_psfile         vul_test_driver test_psfile )
ADD_TEST( vul_test_sprintf        vul_test_driver test_sprintf )
ADD_TEST( vul_test_url            vul_test_driver test_url )
ADD_TEST( vul_test_temp_filename  vul_test_driver test_temp_filename )
