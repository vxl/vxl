INCLUDE_DIRECTORIES( ${allvxl_SOURCE_DIR}/vxl )

ADD_EXECUTABLE( test_vdgl test_vdgl.cxx )
ADD_EXECUTABLE( test1_vdgl test1_vdgl.cxx )

INCLUDE( ${allvxl_SOURCE_DIR}/gel/vdgl/CMakeListsLink.txt )
