# This is mul/vil3d/io/tests/CMakeLists.txt

ADD_EXECUTABLE( vil3d_io_test_driver
  test_driver.cxx
  test_image_view_io.cxx
)
TARGET_LINK_LIBRARIES( vil3d_io_test_driver vil3d_io vpl testlib )

ADD_TEST( vil3d_image_view_io  vil3d_io_test_driver test_image_view_io )
