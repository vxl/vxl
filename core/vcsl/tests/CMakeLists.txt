IF(BUILD_TESTS)
  # test
  ADD_EXECUTABLE( test_meter test_meter.cxx )
  # test
  ADD_EXECUTABLE( test_radian test_radian.cxx )
  # test
  ADD_EXECUTABLE( test_degree test_degree.cxx )
  # test
  ADD_EXECUTABLE( test_cartesian_2d test_cartesian_2d.cxx )
  # test
  ADD_EXECUTABLE( test_cartesian_3d test_cartesian_3d.cxx )
  # test
  ADD_EXECUTABLE( test_polar test_polar.cxx )
  # test
  ADD_EXECUTABLE( test_cylindrical test_cylindrical.cxx )
  # test
  ADD_EXECUTABLE( test_spherical test_spherical.cxx )
  # test
  ADD_EXECUTABLE( test_transformation_graph test_transformation_graph.cxx )
  # test
  ADD_EXECUTABLE( test_composition test_composition.cxx )
  # test
  ADD_EXECUTABLE( test_scale test_scale.cxx )
  # test
  ADD_EXECUTABLE( test_translation test_translation.cxx )
  # test
  ADD_EXECUTABLE( test_rotation test_rotation.cxx )
  # test
  ADD_EXECUTABLE( test_displacement test_displacement.cxx )
ENDIF(BUILD_TESTS)


# Immediate dependencies
INCLUDE( ${allvxl_SOURCE_DIR}/vxl/vcsl/CMakeListsLink.txt )
INCLUDE( ${allvxl_SOURCE_DIR}/vxl/vbl/CMakeListsLink.txt )
INCLUDE( ${allvxl_SOURCE_DIR}/vxl/vnl/CMakeListsLink.txt )
INCLUDE( ${allvxl_SOURCE_DIR}/vcl/CMakeListsLink.txt )
