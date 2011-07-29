# This is core/vnl/xio/tests/CMakeLists.txt

ADD_EXECUTABLE( vnl_xio_test_all
  test_driver.cxx
  test_vnl_xio.cxx
)

TARGET_LINK_LIBRARIES(vnl_xio_test_all vnl_xio testlib)

ADD_TEST( vnl_test_xio      ${EXECUTABLE_OUTPUT_PATH}/vnl_xio_test_all test_vnl_xio )

ADD_EXECUTABLE(vnl_xio_test_include test_include.cxx)
TARGET_LINK_LIBRARIES(vnl_xio_test_include vnl_xio)
ADD_EXECUTABLE(vnl_xio_test_template_include test_template_include.cxx)
TARGET_LINK_LIBRARIES(vnl_xio_test_template_include vnl_xio)
