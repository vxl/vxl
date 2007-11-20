# This is /brl/bbas/bgui3d

INCLUDE_DIRECTORIES( ${GEL_INCLUDE_DIR}/mrc )

# determines if vgui is built and opengl is available
INCLUDE( ${MODULE_PATH}/UseVGUI.cmake )

IF (HAS_VGUI)
  # Optionally Build this library 
  OPTION(BUILD_BGUI3D "Build VGUI wrapper for Coin3d" NO)

  SET (HAS_BGUI3D "NO")
  IF (BUILD_BGUI3D)

    # Find the Coin3D library
    INCLUDE( ${brl_SOURCE_DIR}/bmods/find_coin.cmake )

    IF (COIN3D_FOUND)
      INCLUDE_DIRECTORIES( ${COIN3D_INCLUDE_DIR} )
      SET( HAS_BGUI3D "YES" )
      ADD_DEFINITIONS( -DHAS_BGUI3D )
    ENDIF (COIN3D_FOUND)

    # Find the SimVoleon library
    INCLUDE( ${brl_SOURCE_DIR}/bmods/find_SIMVoleon.cmake )

    IF (SIMVOLEON_FOUND)
      INCLUDE_DIRECTORIES( ${SIMVOLEON_INCLUDE_DIR} )
    ENDIF (SIMVOLEON_FOUND)

  ENDIF (BUILD_BGUI3D)

ENDIF (HAS_VGUI)

# Flag that determines if we were able to successfully build bgui3d.
# Initialize to NO. Change below if yes.
SET(BGUI3D_FOUND "NO" CACHE INTERNAL "Was bgui3d successfully built?" )

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
SET(BGUI3D_FOUND "YES" CACHE INTERNAL "Was bgui3d successfully built?" )
TARGET_LINK_LIBRARIES(bgui3d ${COIN3D_LIBRARY} vgui vpgl vnl vnl_algo)

IF (SIMVOLEON_FOUND)
  TARGET_LINK_LIBRARIES(bgui3d ${SIMVOLEON_LIBRARY})
ENDIF (SIMVOLEON_FOUND)

IF( BUILD_EXAMPLES )
  SUBDIRS( examples )
ENDIF( BUILD_EXAMPLES )

ENDIF (HAS_BGUI3D)
