# This is core/vil1/io/tests/CMakeLists.txt

ADD_EXECUTABLE( vil1_io_test_all
  test_driver.cxx

  golden_test_vil1_io.cxx
  test_memory_image_format_io.cxx
  test_memory_image_impl_io.cxx
  test_memory_image_of_io.cxx
  test_rgb_io.cxx
  test_rgba_io.cxx
)
TARGET_LINK_LIBRARIES( vil1_io_test_all vil1_io testlib vpl )

ADD_TEST( vil1_io_test_memory_image_format_io  ${EXECUTABLE_OUTPUT_PATH}/vil1_io_test_all test_memory_image_format_io )
ADD_TEST( vil1_io_test_memory_image_impl_io    ${EXECUTABLE_OUTPUT_PATH}/vil1_io_test_all test_memory_image_impl_io )
ADD_TEST( vil1_io_test_memory_image_of_io      ${EXECUTABLE_OUTPUT_PATH}/vil1_io_test_all test_memory_image_of_io )
ADD_TEST( vil1_io_test_rgb_io                  ${EXECUTABLE_OUTPUT_PATH}/vil1_io_test_all test_rgb_io )
ADD_TEST( vil1_io_test_rgba_io                 ${EXECUTABLE_OUTPUT_PATH}/vil1_io_test_all test_rgba_io )
ADD_TEST( vil1_io_test_golden                  ${EXECUTABLE_OUTPUT_PATH}/vil1_io_test_all golden_test_vil1_io )

ADD_EXECUTABLE( vil1_io_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( vil1_io_test_include vil1_io )
