ADD_EXECUTABLE( bil_test_all
  test_driver.cxx
  test_bounded_image_view.cxx

)

TARGET_LINK_LIBRARIES( bil_test_all bil vul testlib )
SET( testdata_path ${CMAKE_CURRENT_SOURCE_DIR}/file_read_data )

ADD_TEST( bil_test_bounded_image_view bil_test_all test_bounded_image_view )

ADD_EXECUTABLE(bil_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( bil_test_include bil )
