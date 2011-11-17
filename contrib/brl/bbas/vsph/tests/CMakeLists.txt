# bbas/vsph/tests/CMakeLists.txt
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bbas )

ADD_EXECUTABLE( vsph_test_all
  test_driver.cxx
  test_view_sphere.cxx
  test_spherical_coord.cxx
  test_camera_bounds.cxx
)

TARGET_LINK_LIBRARIES( vsph_test_all vsph testlib vgl vpgl vcl vnl )

ADD_TEST( vsph_test_view_sphere      ${EXECUTABLE_OUTPUT_PATH}/vsph_test_all test_view_sphere )
ADD_TEST( vsph_test_spherical_coord  ${EXECUTABLE_OUTPUT_PATH}/vsph_test_all test_spherical_coord )
ADD_TEST( vsph_test_camera_bounds  ${EXECUTABLE_OUTPUT_PATH}/vsph_test_all test_camera_bounds )

ADD_EXECUTABLE( vsph_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( vsph_test_include vsph)
ADD_EXECUTABLE( vsph_test_template_include test_template_include.cxx )
TARGET_LINK_LIBRARIES( vsph_test_template_include vsph)
