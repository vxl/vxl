ADD_EXECUTABLE( bil_test_all
  test_driver.cxx
  test_bounded_image_view.cxx
  test_raw_image_istream.cxx
)

TARGET_LINK_LIBRARIES( bil_test_all bil vil vul testlib )
SET( testdata_path ${CMAKE_CURRENT_SOURCE_DIR}/file_read_data )

ADD_TEST( bil_test_bounded_image_view ${EXECUTABLE_OUTPUT_PATH}/bil_test_all test_bounded_image_view )
ADD_TEST( bil_test_raw_image_istream  ${EXECUTABLE_OUTPUT_PATH}/bil_test_all test_raw_image_istream )

ADD_EXECUTABLE(bil_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( bil_test_include bil )
ADD_EXECUTABLE(bil_test_template_include test_template_include.cxx )
TARGET_LINK_LIBRARIES( bil_test_template_include bil )
