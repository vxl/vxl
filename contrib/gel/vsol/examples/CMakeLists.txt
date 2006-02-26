INCLUDE_DIRECTORIES( ${VXLCORE_INCLUDE_DIR} )

#ADD_EXECUTABLE( vsol_spatial_object_example vsol_spatial_object_example.cxx )

ADD_EXECUTABLE( vsol_point_example vsol_point_example.cxx )
TARGET_LINK_LIBRARIES( vsol_point_example vsol )

ADD_EXECUTABLE( vsol_conic_example vsol_conic_example.cxx )
TARGET_LINK_LIBRARIES( vsol_conic_example vsol vgl_algo )
