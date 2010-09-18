# vpgl/vsph/tests/CMakeLists.txt

INCLUDE_DIRECTORIES( ${GEL_INCLUDE_DIR}/mrc/vpgl/ )

ADD_EXECUTABLE( vsph_test_all
  test_driver.cxx
  test_view_sphere.cxx
  test_spherical_coord.cxx
)

TARGET_LINK_LIBRARIES( vsph_test_all vsph testlib vgl vpgl vcl vnl)

ADD_TEST( vsph_test_view_sphere      ${EXECUTABLE_OUTPUT_PATH}/vsph_test_all test_view_sphere )
ADD_TEST( vsph_test_spherical_coord  ${EXECUTABLE_OUTPUT_PATH}/vsph_test_all test_spherical_coord )

ADD_EXECUTABLE( vsph_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( vsph_test_include vsph)
