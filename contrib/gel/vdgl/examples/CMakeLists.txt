ADD_EXECUTABLE( example_vdgl test_vdgl.cxx )

# Immediate dependencies
INCLUDE( ${allvxl_SOURCE_DIR}/gel/vtol/CMakeListsLink.txt )
INCLUDE( ${allvxl_SOURCE_DIR}/gel/vdgl/CMakeListsLink.txt )
