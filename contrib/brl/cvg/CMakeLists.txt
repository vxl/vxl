# cvg vgui demos
include( ${VXL_CMAKE_DIR}/UseVGUI.cmake )
include_directories( ${GEL_INCLUDE_DIR}/mrc )
include_directories( ${BRL_INCLUDE_DIR}/bseg/boxm2 )

if( HAS_VGUI )

  set(cvg_view_sources
      cvg_hemisphere_tableau.h    cvg_hemisphere_tableau.cxx )
  aux_source_directory(Templates cvg_view_sources)

  add_library(cvg_view ${cvg_view_sources})
  target_link_libraries(cvg_view vnl vnl_algo vbl_io vgui vpgl vgl vil vbl vul vsph)

  if( BUILD_TESTING )
    add_subdirectory(tests)
  endif( BUILD_TESTING )

  #Create Executables
  add_executable( cvg_hemisphere_view cvg_hemisphere_view.cxx )
  target_link_libraries( cvg_hemisphere_view cvg_view vul vgui vil_pro vpgl_pro vsph)

endif( HAS_VGUI )
