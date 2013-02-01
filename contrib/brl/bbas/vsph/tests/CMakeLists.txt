# bbas/vsph/tests/CMakeLists.txt
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bbas )

ADD_EXECUTABLE( vsph_test_all
  test_driver.cxx
  test_view_sphere.cxx
  test_spherical_coord.cxx
  test_camera_bounds.cxx
  test_sph_geom.cxx
  test_unit_sphere.cxx
#  test_sph_segment.cxx
)

TARGET_LINK_LIBRARIES( vsph_test_all vsph testlib vgl vpgl vcl vnl vpl)

ADD_TEST( vsph_test_view_sphere      ${EXECUTABLE_OUTPUT_PATH}/vsph_test_all test_view_sphere )
ADD_TEST( vsph_test_spherical_coord  ${EXECUTABLE_OUTPUT_PATH}/vsph_test_all test_spherical_coord )
ADD_TEST( vsph_test_camera_bounds  ${EXECUTABLE_OUTPUT_PATH}/vsph_test_all test_camera_bounds )
ADD_TEST( vsph_test_sph_geom  ${EXECUTABLE_OUTPUT_PATH}/vsph_test_all test_sph_geom )
ADD_TEST( vsph_test_unit_sphere  ${EXECUTABLE_OUTPUT_PATH}/vsph_test_all test_unit_sphere )
#ADD_TEST( vsph_test_sph_segment  ${EXECUTABLE_OUTPUT_PATH}/vsph_test_all test_sph_segment )
ADD_EXECUTABLE( vsph_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( vsph_test_include vsph )
ADD_EXECUTABLE( vsph_test_template_include test_template_include.cxx )
TARGET_LINK_LIBRARIES( vsph_test_template_include vsph)
