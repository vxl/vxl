# This is mul/vil3d/io/tests/CMakeLists.txt

ADD_EXECUTABLE( vil3d_io_test_all
  test_driver.cxx
  test_image_view_io.cxx
)
TARGET_LINK_LIBRARIES( vil3d_io_test_all vil3d_io vpl testlib )

ADD_TEST( vil3d_io_test_image_view_io  vil3d_io_test_all test_image_view_io )

ADD_EXECUTABLE( vil3d_io_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( vil3d_io_test_include vil3d_io )
