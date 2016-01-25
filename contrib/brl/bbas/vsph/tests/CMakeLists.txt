# bbas/vsph/tests/CMakeLists.txt
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bbas )

ADD_EXECUTABLE( vsph_test_all
  test_driver.cxx
  test_view_sphere.cxx
  test_spherical_coord.cxx
  test_camera_bounds.cxx
  test_sph_geom.cxx
  test_unit_sphere.cxx
  test_sph_segment.cxx
  test_utils.cxx
  test_grid_index.cxx
  test_sph_cover.cxx
)

TARGET_LINK_LIBRARIES( vsph_test_all vsph testlib vgl bpgl vpgl vcl vnl vpl)

ADD_TEST( NAME vsph_test_view_sphere COMMAND $<TARGET_FILE:vsph_test_all> test_view_sphere )
ADD_TEST( NAME vsph_test_spherical_coord COMMAND $<TARGET_FILE:vsph_test_all> test_spherical_coord )
ADD_TEST( NAME vsph_test_camera_bounds COMMAND $<TARGET_FILE:vsph_test_all> test_camera_bounds )
ADD_TEST( NAME vsph_test_sph_geom COMMAND $<TARGET_FILE:vsph_test_all> test_sph_geom )
ADD_TEST( NAME vsph_test_unit_sphere COMMAND $<TARGET_FILE:vsph_test_all> test_unit_sphere )
ADD_TEST( NAME vsph_test_sph_segment COMMAND $<TARGET_FILE:vsph_test_all> test_sph_segment )
ADD_TEST( NAME vsph_test_utils COMMAND $<TARGET_FILE:vsph_test_all> test_utils )
ADD_TEST( NAME vsph_test_grid_index COMMAND $<TARGET_FILE:vsph_test_all> test_grid_index)
ADD_TEST( NAME vsph_test_sph_cover COMMAND $<TARGET_FILE:vsph_test_all> test_sph_cover)
ADD_EXECUTABLE( vsph_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( vsph_test_include vsph )
ADD_EXECUTABLE( vsph_test_template_include test_template_include.cxx )
TARGET_LINK_LIBRARIES( vsph_test_template_include vsph)
