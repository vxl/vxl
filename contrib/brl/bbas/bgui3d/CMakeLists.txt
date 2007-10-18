# This is /brl/bbas/bgui3d

INCLUDE_DIRECTORIES( ${GEL_INCLUDE_DIR}/mrc )

IF (VGUI_FOUND)
  # Optionally Build this library 
  OPTION(BUILD_BGUI3D "Build VGUI wrapper for Coin3d" NO)
  INCLUDE( ${vxl_SOURCE_DIR}/contrib/brl/bmods/UseBGUI3D.cmake )
ENDIF (VGUI_FOUND)


# If Coin3d was found and building bgui3d
IF (HAS_BGUI3D)
SET(bgui3d_sources
   bgui3d.cxx                      bgui3d.h
   bgui3d_translate_event.cxx      bgui3d_translate_event.h
   bgui3d_file_io.cxx              bgui3d_file_io.h
   bgui3d_algo.cxx                 bgui3d_algo.h

   bgui3d_tableau.cxx              bgui3d_tableau.h              bgui3d_tableau_sptr.h
   bgui3d_viewer_tableau.cxx       bgui3d_viewer_tableau.h       bgui3d_viewer_tableau_sptr.h
   bgui3d_fullviewer_tableau.cxx   bgui3d_fullviewer_tableau.h   bgui3d_fullviewer_tableau_sptr.h
   bgui3d_examiner_tableau.cxx     bgui3d_examiner_tableau.h     bgui3d_examiner_tableau_sptr.h
   bgui3d_project2d_tableau.cxx    bgui3d_project2d_tableau.h    bgui3d_project2d_tableau_sptr.h
   bgui3d_examiner_slider_tableau.cxx    bgui3d_examiner_slider_tableau.h    bgui3d_examiner_slider_tableau_sptr.h
)

ADD_LIBRARY(bgui3d  ${bgui3d_sources})

TARGET_LINK_LIBRARIES(bgui3d vgui vpgl vnl vnl_algo)

IF( BUILD_EXAMPLES )
  SUBDIRS( examples )
ENDIF( BUILD_EXAMPLES )

ENDIF (HAS_BGUI3D)
