# This is core/vil2/io/tests/CMakeLists.txt

ADD_EXECUTABLE( vil2_io_test_all
  test_driver.cxx
  test_memory_chunk_io.cxx
  test_image_view_io.cxx
)
TARGET_LINK_LIBRARIES( vil2_io_test_all vil2_io vpl testlib )

ADD_TEST( vil2_memory_chunk_io  vil2_io_test_all test_memory_chunk_io )
ADD_TEST( vil2_image_view_io    vil2_io_test_all test_image_view_io )

ADD_EXECUTABLE( vil2_io_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( vil2_io_test_include vil2_io )
