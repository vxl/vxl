# vxl/vpgl/xio/tests/CMakeLists.txt

ADD_EXECUTABLE( vpgl_xio_test_all
  test_driver.cxx
)
TARGET_LINK_LIBRARIES(vpgl_xio_test_all vpgl_xio testlib)

#vpgl_xio tests

#vpgl_test_include executable
ADD_EXECUTABLE(vpgl_xio_test_include test_include.cxx)
TARGET_LINK_LIBRARIES(vpgl_xio_test_include vpgl_xio)


