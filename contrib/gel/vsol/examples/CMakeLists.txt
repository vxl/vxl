INCLUDE_DIRECTORIES( ${VXLCORE_INCLUDE_DIR} )

LINK_LIBRARIES( vsol )

ADD_EXECUTABLE( vsol_spatial_object_example vsol_spatial_object_example.cxx )
ADD_EXECUTABLE( vsol_point_example vsol_point_example.cxx )
ADD_EXECUTABLE( vsol_point_example vsol_conic_example.cxx )
