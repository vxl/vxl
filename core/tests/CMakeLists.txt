# This is core/tests/CMakeLists.txt

ADD_EXECUTABLE( vxl_test_all
  test_driver.cxx
  test_config.cxx
)
TARGET_LINK_LIBRARIES( vxl_test_all testlib vcl )

ADD_TEST( vxl_test_config vxl_test_all test_config )
