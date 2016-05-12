# gel/vsol/tests/CMakeLists.txt

add_executable( vsol_test_all
  test_driver.cxx
  test_vsol_point_2d.cxx
  test_vsol_group_2d.cxx
  test_vsol_line_2d.cxx
  test_vsol_conic_2d.cxx
  test_vsol_triangle_2d.cxx
  test_vsol_rectangle_2d.cxx
  test_vsol_polygon_2d.cxx
  test_vsol_digital_curve_2d.cxx
  test_vsol_digital_curve_3d.cxx
  test_vsol_point_3d.cxx
  test_vsol_group_3d.cxx
  test_vsol_line_3d.cxx
  test_vsol_triangle_3d.cxx
  test_vsol_rectangle_3d.cxx
  test_vsol_polygon_3d.cxx
  test_vsol_tetrahedron.cxx
  test_vsol_io.cxx
)
target_link_libraries( vsol_test_all vsol ${VXL_LIB_PREFIX}vnl ${VXL_LIB_PREFIX}vgl ${VXL_LIB_PREFIX}vbl_io ${VXL_LIB_PREFIX}vsl ${VXL_LIB_PREFIX}vpl ${VXL_LIB_PREFIX}testlib )

add_test( NAME vsol_test_point_2d COMMAND $<TARGET_FILE:vsol_test_all> test_vsol_point_2d)
add_test( NAME vsol_test_group_2d COMMAND $<TARGET_FILE:vsol_test_all> test_vsol_group_2d)
add_test( NAME vsol_test_line_2d COMMAND $<TARGET_FILE:vsol_test_all> test_vsol_line_2d)
add_test( NAME vsol_test_conic_2d COMMAND $<TARGET_FILE:vsol_test_all> test_vsol_conic_2d)
add_test( NAME vsol_test_triangle_2d COMMAND $<TARGET_FILE:vsol_test_all> test_vsol_triangle_2d)
add_test( NAME vsol_test_rectangle_2d COMMAND $<TARGET_FILE:vsol_test_all> test_vsol_rectangle_2d)
add_test( NAME vsol_test_polygon_2d COMMAND $<TARGET_FILE:vsol_test_all> test_vsol_polygon_2d)
add_test( NAME vsol_test_digital_curve_2d COMMAND $<TARGET_FILE:vsol_test_all> test_vsol_digital_curve_2d)
add_test( NAME vsol_test_digital_curve_3d COMMAND $<TARGET_FILE:vsol_test_all> test_vsol_digital_curve_3d)
add_test( NAME vsol_test_point_3d COMMAND $<TARGET_FILE:vsol_test_all> test_vsol_point_3d)
add_test( NAME vsol_test_group_3d COMMAND $<TARGET_FILE:vsol_test_all> test_vsol_group_3d)
add_test( NAME vsol_test_line_3d COMMAND $<TARGET_FILE:vsol_test_all> test_vsol_line_3d)
add_test( NAME vsol_test_triangle_3d COMMAND $<TARGET_FILE:vsol_test_all> test_vsol_triangle_3d)
add_test( NAME vsol_test_rectangle_3d COMMAND $<TARGET_FILE:vsol_test_all> test_vsol_rectangle_3d)
add_test( NAME vsol_test_polygon_3d COMMAND $<TARGET_FILE:vsol_test_all> test_vsol_polygon_3d)
add_test( NAME vsol_test_tetrahedron COMMAND $<TARGET_FILE:vsol_test_all> test_vsol_tetrahedron)
add_test( NAME vsol_test_io COMMAND $<TARGET_FILE:vsol_test_all> test_vsol_io)

add_executable( vsol_test_include test_include.cxx )
target_link_libraries( vsol_test_include vsol )
