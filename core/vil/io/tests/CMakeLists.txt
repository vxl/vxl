# This is core/vil/io/tests/CMakeLists.txt

ADD_EXECUTABLE( vil_io_test_all
  test_driver.cxx
  test_memory_chunk_io.cxx
  test_image_view_io.cxx
)
TARGET_LINK_LIBRARIES( vil_io_test_all vil_io vpl testlib )

ADD_TEST( vil_io_test_memory_chunk_io  vil_io_test_all test_memory_chunk_io )
ADD_TEST( vil_io_test_image_view_io    vil_io_test_all test_image_view_io )

ADD_EXECUTABLE( vil_io_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( vil_io_test_include vil_io )
