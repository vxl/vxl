# cvg vgui demos
INCLUDE( ${VXL_CMAKE_DIR}/UseVGUI.cmake )
INCLUDE_DIRECTORIES( ${GEL_INCLUDE_DIR}/mrc )
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bseg/boxm2 )

IF( HAS_VGUI )

  SET(cvg_view_sources
      cvg_hemisphere_tableau.h    cvg_hemisphere_tableau.cxx )
  AUX_SOURCE_DIRECTORY(Templates cvg_view_sources)

  ADD_LIBRARY(cvg_view ${cvg_view_sources})
  TARGET_LINK_LIBRARIES(cvg_view bgeo vnl vnl_algo vbl_io vpgl vgl vil vbl vul vsph)


  #Create Executables
  #LINK_LIBRARIES( vgui )
  ADD_EXECUTABLE( cvg_hemisphere_view cvg_hemisphere_view.cxx )
  TARGET_LINK_LIBRARIES( cvg_hemisphere_view cvg_view vul vgui vil_pro vpgl_pro vsph)

ENDIF( HAS_VGUI )
