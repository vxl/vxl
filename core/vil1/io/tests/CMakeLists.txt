# ./vxl/vil/io/tests/CMakeLists.txt

ADD_EXECUTABLE( vil_io_test_driver
  test_driver.cxx

  golden_test_vil_io.cxx
  test_include.cxx
  test_memory_image_format_io.cxx
  test_memory_image_impl_io.cxx
  test_memory_image_of_io.cxx
  test_rgb_io.cxx
  test_rgba_io.cxx
)
TARGET_LINK_LIBRARIES( vil_io_test_driver vil_io testlib )

ADD_TEST( vil_io_test_include                 vil_io_test_driver test_include )
ADD_TEST( vil_io_test_memory_image_format_io  vil_io_test_driver test_memory_image_format_io )
ADD_TEST( vil_io_test_memory_image_impl_io    vil_io_test_driver test_memory_image_impl_io )
ADD_TEST( vil_io_test_memory_image_of_io      vil_io_test_driver test_memory_image_of_io )
ADD_TEST( vil_io_test_rgb_io                  vil_io_test_driver test_rgb_io )
ADD_TEST( vil_io_test_rgba_io                 vil_io_test_driver test_rgba_io )
ADD_TEST( vil_io_test_golden                  vil_io_test_driver golden_test_vil_io )
