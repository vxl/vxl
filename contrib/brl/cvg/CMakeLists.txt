# cvg vgui demos
include( ${VXL_CMAKE_DIR}/UseVGUI.cmake )
include_directories( ${GEL_INCLUDE_DIR}/mrc )
include_directories( ${BRL_INCLUDE_DIR}/bseg/boxm2 )

if( HAS_VGUI )

  set(cvg_view_sources
      cvg_hemisphere_tableau.h    cvg_hemisphere_tableau.cxx )
  aux_source_directory(Templates cvg_view_sources)

  vxl_add_library(LIBRARY_NAME cvg_view LIBRARY_SOURCES ${cvg_view_sources})
  target_link_libraries(cvg_view ${VXL_LIB_PREFIX}vnl ${VXL_LIB_PREFIX}vnl_algo ${VXL_LIB_PREFIX}vbl_io ${VXL_LIB_PREFIX}vgui ${VXL_LIB_PREFIX}vpgl ${VXL_LIB_PREFIX}vgl ${VXL_LIB_PREFIX}vil ${VXL_LIB_PREFIX}vbl ${VXL_LIB_PREFIX}vul vsph)

  if( BUILD_TESTING )
    add_subdirectory(tests)
  endif()

  #Create Executables
  add_executable( cvg_hemisphere_view cvg_hemisphere_view.cxx )
  target_link_libraries( cvg_hemisphere_view cvg_view ${VXL_LIB_PREFIX}vul ${VXL_LIB_PREFIX}vgui vil_pro vpgl_pro vsph)

endif()
