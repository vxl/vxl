ADD_EXECUTABLE( bsta_io_test_all
  test_driver.cxx
  test_bsta_histogram_io.cxx
)
TARGET_LINK_LIBRARIES( bsta_io_test_all bsta_io bsta vnl vpl vbl_io vsl vbl vgl testlib )
ADD_TEST( NAME bsta_test_histogram_io COMMAND $<TARGET_FILE:bsta_io_test_all> test_bsta_histogram_io )

ADD_EXECUTABLE( bsta_io_test_include test_include.cxx )
TARGET_LINK_LIBRARIES(bsta_io_test_include bsta_io )
