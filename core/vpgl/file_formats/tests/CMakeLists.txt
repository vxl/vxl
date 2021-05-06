add_executable( vpgl_file_formats_test_all
  test_driver.cxx

  test_geo_camera.cxx
)

target_link_libraries( vpgl_file_formats_test_all ${VXL_LIB_PREFIX}vpgl_file_formats ${VXL_LIB_PREFIX}testlib )
add_test( NAME vpgl_file_formats_test_geo_camera COMMAND $<TARGET_FILE:vpgl_file_formats_test_all> test_geo_camera)

add_executable( vpgl_file_formats_test_include test_include.cxx )
target_link_libraries( vpgl_file_formats_test_include ${VXL_LIB_PREFIX}vpgl_file_formats)
