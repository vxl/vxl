add_executable( bsta_io_test_all
  test_driver.cxx
  test_bsta_histogram_io.cxx
)
target_link_libraries( bsta_io_test_all bsta_io bsta ${VXL_LIB_PREFIX}vnl ${VXL_LIB_PREFIX}vpl ${VXL_LIB_PREFIX}vbl_io ${VXL_LIB_PREFIX}vsl ${VXL_LIB_PREFIX}vbl ${VXL_LIB_PREFIX}vgl ${VXL_LIB_PREFIX}testlib )
add_test( NAME bsta_test_histogram_io COMMAND $<TARGET_FILE:bsta_io_test_all> test_bsta_histogram_io )

add_executable( bsta_io_test_include test_include.cxx )
target_link_libraries(bsta_io_test_include bsta_io )
