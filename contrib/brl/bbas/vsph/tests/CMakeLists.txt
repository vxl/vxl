# bbas/vsph/tests/CMakeLists.txt
include_directories( ${BRL_INCLUDE_DIR}/bbas )

add_executable( vsph_test_all
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

target_link_libraries( vsph_test_all vsph ${VXL_LIB_PREFIX}testlib ${VXL_LIB_PREFIX}vgl bpgl ${VXL_LIB_PREFIX}vpgl ${VXL_LIB_PREFIX}vcl ${VXL_LIB_PREFIX}vnl ${VXL_LIB_PREFIX}vpl)

add_test( NAME vsph_test_view_sphere COMMAND $<TARGET_FILE:vsph_test_all> test_view_sphere )
add_test( NAME vsph_test_spherical_coord COMMAND $<TARGET_FILE:vsph_test_all> test_spherical_coord )
add_test( NAME vsph_test_camera_bounds COMMAND $<TARGET_FILE:vsph_test_all> test_camera_bounds )
add_test( NAME vsph_test_sph_geom COMMAND $<TARGET_FILE:vsph_test_all> test_sph_geom )
add_test( NAME vsph_test_unit_sphere COMMAND $<TARGET_FILE:vsph_test_all> test_unit_sphere )
add_test( NAME vsph_test_sph_segment COMMAND $<TARGET_FILE:vsph_test_all> test_sph_segment )
add_test( NAME vsph_test_utils COMMAND $<TARGET_FILE:vsph_test_all> test_utils )
add_test( NAME vsph_test_grid_index COMMAND $<TARGET_FILE:vsph_test_all> test_grid_index)
add_test( NAME vsph_test_sph_cover COMMAND $<TARGET_FILE:vsph_test_all> test_sph_cover)
add_executable( vsph_test_include test_include.cxx )
target_link_libraries( vsph_test_include vsph )
add_executable( vsph_test_template_include test_template_include.cxx )
target_link_libraries( vsph_test_template_include vsph)
