INCLUDE_DIRECTORIES( ${allvxl_SOURCE_DIR}/vxl )

IF(BUILD_TESTING)
  # test
  ADD_EXECUTABLE( test_gevd_param_mixin test_gevd_param_mixin.cxx )
  # test
  ADD_EXECUTABLE( test_gevd_memory_mixin test_gevd_memory_mixin.cxx )
# test
  ADD_EXECUTABLE( test_gevd_bufferxy test_gevd_bufferxy.cxx )
ENDIF(BUILD_TESTING)


# Immediate dependencies
INCLUDE( ${allvxl_SOURCE_DIR}/gel/gevd/CMakeListsLink.txt )
INCLUDE( ${allvxl_SOURCE_DIR}/vxl/vgl/algo/CMakeListsLink.txt )
INCLUDE( ${allvxl_SOURCE_DIR}/vxl/vnl/algo/CMakeListsLink.txt )
INCLUDE( ${allvxl_SOURCE_DIR}/vxl/vbl/CMakeListsLink.txt )
INCLUDE( ${allvxl_SOURCE_DIR}/vxl/vul/CMakeListsLink.txt )
INCLUDE( ${allvxl_SOURCE_DIR}/vcl/CMakeListsLink.txt )
