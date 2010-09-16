# vpgl/vsph/tests/CMakeLists.txt

INCLUDE_DIRECTORIES( ${GEL_INCLUDE_DIR}/mrc/vpgl/ )

ADD_EXECUTABLE( vsph_test_all
  test_driver.cxx
  test_view_sphere.cxx
  test_spherical_coord.cxx
)


TARGET_LINK_LIBRARIES( vsph_test_all vsph testlib vgl vpgl vcl vnl)
ADD_TEST( test_view_sphere  vsph_test_all test_view_sphere )

TARGET_LINK_LIBRARIES( vsph_test_all vsph testlib vgl vpgl vcl vnl)
ADD_TEST( test_spherical_coord  vsph_test_all test_spherical_coord )

ADD_EXECUTABLE( vsph_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( vsph_test_include vsph)
