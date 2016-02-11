add_executable( bil_test_all
  test_driver.cxx
  test_bounded_image_view.cxx
  test_raw_image_istream.cxx
)

target_link_libraries( bil_test_all bil ${VXL_LIB_PREFIX}vil ${VXL_LIB_PREFIX}vul ${VXL_LIB_PREFIX}testlib )
set( testdata_path ${CMAKE_CURRENT_SOURCE_DIR}/file_read_data )

add_test( NAME bil_test_bounded_image_view COMMAND $<TARGET_FILE:bil_test_all> test_bounded_image_view )
add_test( NAME bil_test_raw_image_istream COMMAND $<TARGET_FILE:bil_test_all> test_raw_image_istream )

add_executable(bil_test_include test_include.cxx )
target_link_libraries( bil_test_include bil )
add_executable(bil_test_template_include test_template_include.cxx )
target_link_libraries( bil_test_template_include bil )
