# This is mul/vil2/io/tests/CMakeLists.txt

ADD_EXECUTABLE( vil2_io_test_driver
  test_driver.cxx
  test_memory_chunk_io.cxx
  test_image_view_io.cxx
)
TARGET_LINK_LIBRARIES( vil2_io_test_driver vil2_io vpl testlib )

ADD_TEST( vil2_memory_chunk_io  vil2_io_test_driver test_memory_chunk_io )
ADD_TEST( vil2_image_view_io    vil2_io_test_driver test_image_view_io )
