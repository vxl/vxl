
add_executable(vgl_conic_example vgl_conic_example.cxx)
target_link_libraries( vgl_conic_example ${VXL_LIB_PREFIX}vgl_algo )

if(VXL_BUILD_VGUI)
  include( ${VXL_CMAKE_DIR}/UseVGUI.cmake )
  if( VGUI_FOUND )
    add_executable(vgl_calculate_homography calculate_homography.cxx)
    target_link_libraries( vgl_calculate_homography ${VXL_LIB_PREFIX}vgl_algo ${VXL_LIB_PREFIX}vgui ${VXL_LIB_PREFIX}vnl )
  endif()
endif()
