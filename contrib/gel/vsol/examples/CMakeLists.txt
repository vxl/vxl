INCLUDE_DIRECTORIES( ${allvxl_SOURCE_DIR}/vxl )

ADD_EXECUTABLE( vsol_spatial_object_example vsol_spatial_object_example )
ADD_EXECUTABLE( vsol_point_example vsol_point_example )
ADD_EXECUTABLE( vsol_point_example vsol_conic_example )

# Immediate dependencies
INCLUDE( ${allvxl_SOURCE_DIR}/gel/vsol/CMakeListsLink.txt )
