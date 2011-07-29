# This is core/vgl/xio/tests/CMakeLists.txt

ADD_EXECUTABLE( vgl_xio_test_all
  test_driver.cxx
  test_vgl_xio.cxx
)

TARGET_LINK_LIBRARIES(vgl_xio_test_all vgl_xio testlib)

ADD_TEST( vgl_test_xio      ${EXECUTABLE_OUTPUT_PATH}/vgl_xio_test_all test_vgl_xio )

ADD_EXECUTABLE(vgl_xio_test_include test_include.cxx)
TARGET_LINK_LIBRARIES(vgl_xio_test_include vgl_xio)
ADD_EXECUTABLE(vgl_xio_test_template_include test_template_include.cxx)
TARGET_LINK_LIBRARIES(vgl_xio_test_template_include vgl_xio)
