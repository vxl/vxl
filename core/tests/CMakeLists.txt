# vxl/tests/CMakeLists.txt

ADD_EXECUTABLE( vxl_test_driver
  test_driver.cxx
  test_config.cxx
)
TARGET_LINK_LIBRARIES( vxl_test_driver testlib vcl )

ADD_TEST( vxl_test_config vxl_test_driver test_config )
