INCLUDE_DIRECTORIES( ${allvxl_SOURCE_DIR}/vxl )

IF(BUILD_TESTING)
  # test
  ADD_EXECUTABLE( test_vsol_point_2d test_vsol_point_2d.cxx )
  # test
  ADD_EXECUTABLE( test_vsol_group_2d test_vsol_group_2d.cxx )
  # test
  ADD_EXECUTABLE( test_vsol_line_2d test_vsol_line_2d.cxx )
  # test
  ADD_EXECUTABLE( test_vsol_conic_2d test_vsol_conic_2d.cxx )
  # test
  ADD_EXECUTABLE( test_vsol_triangle_2d test_vsol_triangle_2d.cxx )
  # test
  ADD_EXECUTABLE( test_vsol_rectangle_2d test_vsol_rectangle_2d.cxx )
  # test
  ADD_EXECUTABLE( test_vsol_polygon_2d test_vsol_polygon_2d.cxx )
  # test
  ADD_EXECUTABLE( test_vsol_point_3d test_vsol_point_3d.cxx )
  # test
  ADD_EXECUTABLE( test_vsol_group_3d test_vsol_group_3d.cxx )
  # test
  ADD_EXECUTABLE( test_vsol_line_3d test_vsol_line_3d.cxx )
  # test
  ADD_EXECUTABLE( test_vsol_triangle_3d test_vsol_triangle_3d.cxx )
  # test
  ADD_EXECUTABLE( test_vsol_rectangle_3d test_vsol_rectangle_3d.cxx )
  # test
  ADD_EXECUTABLE( test_vsol_polygon_3d test_vsol_polygon_3d.cxx )
  # test
  ADD_EXECUTABLE( test_vsol_tetrahedron test_vsol_tetrahedron.cxx )
ENDIF(BUILD_TESTING)


# Immediate dependencies
INCLUDE( ${allvxl_SOURCE_DIR}/gel/vsol/CMakeListsLink.txt )
INCLUDE( ${allvxl_SOURCE_DIR}/vxl/vgl/algo/CMakeListsLink.txt )
INCLUDE( ${allvxl_SOURCE_DIR}/vxl/vnl/algo/CMakeListsLink.txt )
INCLUDE( ${allvxl_SOURCE_DIR}/vxl/vbl/CMakeListsLink.txt )
INCLUDE( ${allvxl_SOURCE_DIR}/vxl/vul/CMakeListsLink.txt )
INCLUDE( ${allvxl_SOURCE_DIR}/vcl/CMakeListsLink.txt )
