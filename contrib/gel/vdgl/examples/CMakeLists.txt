INCLUDE_DIRECTORIES( ${allvxl_SOURCE_DIR}/vxl )
INCLUDE_DIRECTORIES( ${allvxl_SOURCE_DIR}/gel )

ADD_EXECUTABLE( example_vdgl test_vdgl )

# Immediate dependencies
INCLUDE( ${allvxl_SOURCE_DIR}/gel/vtol/CMakeListsLink.txt )
INCLUDE( ${allvxl_SOURCE_DIR}/gel/vdgl/CMakeListsLink.txt )

