INCLUDE_DIRECTORIES( ${allvxl_SOURCE_DIR}/vxl )
INCLUDE_DIRECTORIES( ${allvxl_SOURCE_DIR}/gel )

ADD_EXECUTABLE( test_vdgl test_vdgl )

# Immediate dependencies
INCLUDE( ${allvxl_SOURCE_DIR}/gel/vtol/CMakeListsLink.txt )
LINK_LIBRARIES(vdgl)
LINK_DIRECTORIES( ${allvxl_BINARY_DIR}/gel/vdgl )

