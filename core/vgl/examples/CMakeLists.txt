INCLUDE( ${MODULE_PATH}/UseVGUI.cmake )

ADD_EXECUTABLE(vgl_conic_example vgl_conic_example.cxx)
TARGET_LINK_LIBRARIES( vgl_conic_example vgl_algo )

IF( VGUI_FOUND )
  ADD_EXECUTABLE(vgl_calculate_homography calculate_homography.cxx)
  TARGET_LINK_LIBRARIES( vgl_calculate_homography vgl_algo vgui vnl )
ENDIF( VGUI_FOUND )
