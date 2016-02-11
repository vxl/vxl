# This is mul/vil3d/io/tests/CMakeLists.txt

add_executable( vil3d_io_test_all
  test_driver.cxx
  test_image_view_io.cxx
)
target_link_libraries( vil3d_io_test_all vil3d_io vil3d ${VXL_LIB_PREFIX}vpl ${VXL_LIB_PREFIX}testlib )

add_test( NAME vil3d_io_test_image_view_io COMMAND $<TARGET_FILE:vil3d_io_test_all> test_image_view_io )

add_executable( vil3d_io_test_include test_include.cxx )
target_link_libraries( vil3d_io_test_include vil3d_io )
