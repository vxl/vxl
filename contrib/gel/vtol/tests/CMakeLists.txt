INCLUDE_DIRECTORIES( ${allvxl_SOURCE_DIR}/vxl )

IF(BUILD_TESTING)
  # test
  ADD_EXECUTABLE( test_vertex_2d test_vertex_2d.cxx )
  # test
  ADD_EXECUTABLE( test_zero_chain test_zero_chain.cxx )
  # test
  ADD_EXECUTABLE( test_edge_2d test_edge_2d.cxx )
  # test
  ADD_EXECUTABLE( test_one_chain test_one_chain.cxx )
  # test
  ADD_EXECUTABLE( test_face_2d test_face_2d.cxx )
  # test
  ADD_EXECUTABLE( test_two_chain test_two_chain.cxx )
  # test
  ADD_EXECUTABLE( test_block test_block.cxx )
ENDIF(BUILD_TESTING)


# Immediate dependencies
INCLUDE( ${allvxl_SOURCE_DIR}/gel/vtol/CMakeListsLink.txt )
INCLUDE( ${allvxl_SOURCE_DIR}/gel/vsol/CMakeListsLink.txt )
INCLUDE( ${allvxl_SOURCE_DIR}/vxl/vnl/CMakeListsLink.txt )
INCLUDE( ${allvxl_SOURCE_DIR}/vxl/vbl/CMakeListsLink.txt )
INCLUDE( ${allvxl_SOURCE_DIR}/vxl/vgl/CMakeListsLink.txt )
INCLUDE( ${allvxl_SOURCE_DIR}/vxl/vul/CMakeListsLink.txt )
INCLUDE( ${allvxl_SOURCE_DIR}/vcl/CMakeListsLink.txt )
