# bbas/bpgl/tests/CMakeLists.txt

ADD_EXECUTABLE( bpgl_test_all
  test_driver.cxx
  test_segmented_rolling_shutter_camera.cxx
  test_camera_utils.cxx
)

TARGET_LINK_LIBRARIES( bpgl_test_all bpgl testlib vgl vpgl vcl vnl)

ADD_TEST( NAME vpgl_test_segmented_rolling_shutter_camera COMMAND $<TARGET_FILE:bpgl_test_all> test_segmented_rolling_shutter_camera)
ADD_TEST( NAME vpgl_test_camera_utils COMMAND $<TARGET_FILE:bpgl_test_all> test_camera_utils)

ADD_EXECUTABLE( bpgl_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( bpgl_test_include bpgl)
ADD_EXECUTABLE( bpgl_test_template_include test_template_include.cxx )
TARGET_LINK_LIBRARIES( bpgl_test_template_include bpgl)
