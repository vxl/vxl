# vxl/vpgl/xio/tests/CMakeLists.txt

add_executable( vpgl_xio_test_all
  test_driver.cxx
)
target_link_libraries(vpgl_xio_test_all ${VXL_LIB_PREFIX}vpgl_xio ${VXL_LIB_PREFIX}testlib)

#vpgl_xio tests

#vpgl_test_include executable
add_executable(vpgl_xio_test_include test_include.cxx)
target_link_libraries(vpgl_xio_test_include ${VXL_LIB_PREFIX}vpgl_xio)
