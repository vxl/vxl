IF(BUILD_TESTS)
  # test
  ADD_EXECUTABLE( test_meter test_meter )
  # test
  ADD_EXECUTABLE( test_radian test_radian )
  # test
  ADD_EXECUTABLE( test_degree test_degree )
  # test
  ADD_EXECUTABLE( test_cartesian_2d test_cartesian_2d )
  # test
  ADD_EXECUTABLE( test_cartesian_3d test_cartesian_3d )
  # test
  ADD_EXECUTABLE( test_polar test_polar )
  # test
  ADD_EXECUTABLE( test_cylindrical test_cylindrical )
  # test
  ADD_EXECUTABLE( test_spherical test_spherical )
  # test
  ADD_EXECUTABLE( test_transformation_graph test_transformation_graph )
  # test
  ADD_EXECUTABLE( test_composition test_composition )
  # test
  ADD_EXECUTABLE( test_scale test_scale )
  # test
  ADD_EXECUTABLE( test_translation test_translation )
  # test
  ADD_EXECUTABLE( test_rotation test_rotation )
  # test
  ADD_EXECUTABLE( test_displacement test_displacement )
ENDIF(BUILD_TESTS)


# Immediate dependencies
INCLUDE( ${allvxl_SOURCE_DIR}/vxl/vcsl/CMakeListsLink.txt )
INCLUDE( ${allvxl_SOURCE_DIR}/vxl/vbl/CMakeListsLink.txt )
INCLUDE( ${allvxl_SOURCE_DIR}/vxl/vnl/CMakeListsLink.txt )
INCLUDE( ${allvxl_SOURCE_DIR}/vcl/CMakeListsLink.txt )
