# This is core/vil/io/tests/CMakeLists.txt

ADD_EXECUTABLE( vil_io_test_all
  test_driver.cxx

  golden_test_vil_io.cxx
  test_memory_image_format_io.cxx
  test_memory_image_impl_io.cxx
  test_memory_image_of_io.cxx
  test_rgb_io.cxx
  test_rgba_io.cxx
)
TARGET_LINK_LIBRARIES( vil_io_test_all vil_io testlib vpl )

ADD_TEST( vil_io_test_memory_image_format_io  vil_io_test_all test_memory_image_format_io )
ADD_TEST( vil_io_test_memory_image_impl_io    vil_io_test_all test_memory_image_impl_io )
ADD_TEST( vil_io_test_memory_image_of_io      vil_io_test_all test_memory_image_of_io )
ADD_TEST( vil_io_test_rgb_io                  vil_io_test_all test_rgb_io )
ADD_TEST( vil_io_test_rgba_io                 vil_io_test_all test_rgba_io )
ADD_TEST( vil_io_test_golden                  vil_io_test_all golden_test_vil_io )

ADD_EXECUTABLE( vil_io_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( vil_io_test_include vil_io )
