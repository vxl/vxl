INCLUDE_DIRECTORIES( ${allvxl_SOURCE_DIR}/vxl )

ADD_EXECUTABLE( test_vdgl test_vdgl.cxx )
ADD_EXECUTABLE( test1_vdgl test1_vdgl.cxx )

# Immediate dependencies
INCLUDE( ${allvxl_SOURCE_DIR}/gel/vsol/CMakeListsLink.txt )
INCLUDE( ${allvxl_SOURCE_DIR}/vxl/vnl/algo/CMakeListsLink.txt )
INCLUDE( ${allvxl_SOURCE_DIR}/vcl/CMakeListsLink.txt )
LINK_LIBRARIES( vdgl)
LINK_DIRECTORIES(
  ${allvxl_BINARY_DIR}/gel/vdgl
)
