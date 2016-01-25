# This is core/vil/io/tests/CMakeLists.txt

add_executable( vil_io_test_all
  test_driver.cxx
  test_memory_chunk_io.cxx
  test_image_view_io.cxx
)
target_link_libraries( vil_io_test_all vil_io vpl testlib )

add_test( NAME vil_io_test_memory_chunk_io COMMAND $<TARGET_FILE:vil_io_test_all> test_memory_chunk_io )
add_test( NAME vil_io_test_image_view_io COMMAND $<TARGET_FILE:vil_io_test_all> test_image_view_io )

add_executable( vil_io_test_include test_include.cxx )
target_link_libraries( vil_io_test_include vil_io )
add_executable( vil_io_test_template_include test_template_include.cxx )
target_link_libraries( vil_io_test_template_include vil_io )
