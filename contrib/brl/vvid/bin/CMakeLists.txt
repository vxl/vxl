# brl/vvid/bin/CMakeLists.txt

INCLUDE_DIRECTORIES( ${VXLCORE_INCLUDE_DIR} )

INCLUDE( ${MODULE_PATH}/FindMPEG2.cmake )

IF( MPEG2_FOUND )
  INCLUDE_DIRECTORIES(${MPEG2_INCLUDE_DIR})
  ADD_DEFINITIONS(-DHAS_MPEG2)
ENDIF( MPEG2_FOUND )

SET(vidfpl_sources vidfpl.cxx vidfpl_menus.cxx vidfpl_menus.h)
SET(cl_grid_finder_sources command_line_grid_finder.cxx)

ADD_EXECUTABLE (vidfpl ${vidfpl_sources})
ADD_EXECUTABLE (cl_grid_finder ${cl_grid_finder_sources})

TARGET_LINK_LIBRARIES(vidfpl brip  vvid vidl_vil1 vgui )

IF(HAS_CMU1394CAMERA)
 SET(vidster_sources vidster.cxx vidster_menus.cxx vidster_menus.h)
 ADD_EXECUTABLE (vidster ${vidster_sources})
 TARGET_LINK_LIBRARIES(vidster vvid vgui )

 SET(videx_sources videx.cxx videx_menus.cxx videx_menus.h)
 ADD_EXECUTABLE (videx ${videx_sources})
 TARGET_LINK_LIBRARIES(videx vvid bgui vgui vpro)
ENDIF(HAS_CMU1394CAMERA)
