# bbas/bpgl/tests/CMakeLists.txt

add_executable( bpgl_test_all
  test_driver.cxx
  test_segmented_rolling_shutter_camera.cxx
  test_camera_utils.cxx
)

target_link_libraries( bpgl_test_all bpgl ${VXL_LIB_PREFIX}testlib ${VXL_LIB_PREFIX}vgl ${VXL_LIB_PREFIX}vpgl ${VXL_LIB_PREFIX}vcl ${VXL_LIB_PREFIX}vnl)

add_test( NAME vpgl_test_segmented_rolling_shutter_camera COMMAND $<TARGET_FILE:bpgl_test_all> test_segmented_rolling_shutter_camera)
add_test( NAME vpgl_test_camera_utils COMMAND $<TARGET_FILE:bpgl_test_all> test_camera_utils)

add_executable( bpgl_test_include test_include.cxx )
target_link_libraries( bpgl_test_include bpgl)
add_executable( bpgl_test_template_include test_template_include.cxx )
target_link_libraries( bpgl_test_template_include bpgl)
