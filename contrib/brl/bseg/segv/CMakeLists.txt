# segmentation view library
# brl/bseg/segv/CMakeLists.txt
INCLUDE_DIRECTORIES(${BRL_INCLUDE_DIR}/bseg)
INCLUDE_DIRECTORIES(${BRL_INCLUDE_DIR}/bbas)
INCLUDE_DIRECTORIES(${GEL_INCLUDE_DIR})

# determines if vgui is built and opengl is available
INCLUDE( ${MODULE_PATH}/UseVGUI.cmake )

IF(HAS_VGUI)

 SET(segv_sources
  segv_segmentation_manager.cxx segv_segmentation_manager.h
  segv_menus.cxx                segv_menus.h
  segv_vtol2D_tableau.cxx       segv_vtol2D_tableau.h segv_vtol2D_tableau_sptr.h
  segv_vtol_soview2D.cxx        segv_vtol_soview2D.h
 )
 AUX_SOURCE_DIRECTORY(Templates segv_sources)

 ADD_LIBRARY(segv ${segv_sources})

 TARGET_LINK_LIBRARIES(segv bgui vgui strk sdet bsol brip vtol vdgl vsol vil1 vbl)

 IF (BUILD_TESTING)
  SUBDIRS(tests)
 ENDIF (BUILD_TESTING)

ENDIF(HAS_VGUI)
