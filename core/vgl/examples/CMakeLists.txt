include( ${VXL_CMAKE_DIR}/UseVGUI.cmake )

add_executable(vgl_conic_example vgl_conic_example.cxx)
target_link_libraries( vgl_conic_example vgl_algo )

if( VGUI_FOUND )
  add_executable(vgl_calculate_homography calculate_homography.cxx)
  target_link_libraries( vgl_calculate_homography vgl_algo vgui vnl )
endif()
