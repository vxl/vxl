# bbas/bpgl/tests/CMakeLists.txt

ADD_EXECUTABLE( bpgl_test_all
  test_driver.cxx
  test_segmented_rolling_shutter_camera.cxx
  test_poly_radial_distortion.cxx
  test_kml_camera.cxx
)

TARGET_LINK_LIBRARIES( bpgl_test_all bpgl testlib vgl vpgl vcl vnl)

ADD_TEST(vpgl_test_segmented_rolling_shutter_camera ${EXECUTABLE_OUTPUT_PATH}/bpgl_test_all test_segmented_rolling_shutter_camera)
ADD_TEST(vpgl_test_poly_radial_distortion           ${EXECUTABLE_OUTPUT_PATH}/bpgl_test_all test_poly_radial_distortion)
ADD_TEST(vpgl_test_kml_camera           ${EXECUTABLE_OUTPUT_PATH}/bpgl_test_all test_kml_camera)

ADD_EXECUTABLE( bpgl_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( bpgl_test_include bpgl)
ADD_EXECUTABLE( bpgl_test_template_include test_template_include.cxx )
TARGET_LINK_LIBRARIES( bpgl_test_template_include bpgl)
