ADD_EXECUTABLE(vgl_conic_example vgl_conic_example.cxx)
TARGET_LINK_LIBRARIES( vgl_conic_example vgl_algo )

ADD_EXECUTABLE(vgl_calculate_homography calculate_homography.cxx)
TARGET_LINK_LIBRARIES( vgl_calculate_homography vgl_algo vgui vnl )
