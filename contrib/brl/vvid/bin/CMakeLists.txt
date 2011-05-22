# brl/vvid/bin/CMakeLists.txt

INCLUDE_DIRECTORIES( ${VXLCORE_INCLUDE_DIR} )

INCLUDE( ${VXL_CMAKE_DIR}/FindMPEG2.cmake )

IF( MPEG2_FOUND )
  INCLUDE_DIRECTORIES(${MPEG2_INCLUDE_DIR})
  ADD_DEFINITIONS(-DHAS_MPEG2)
ENDIF( MPEG2_FOUND )

IF (BUILD_UNMAINTAINED_LIBRARIES OR BUILD_FOR_VXL_DASHBOARD)
 # depending on vidl_vil1
 SET(vidfpl_sources vidfpl.cxx vidfpl_menus.cxx vidfpl_menus.h)
 ADD_EXECUTABLE (vidfpl ${vidfpl_sources})
 TARGET_LINK_LIBRARIES(vidfpl vvid vidl_vil1 vgui)

 # depending on vidl1
 SET(vidfpl_vil_sources vidfpl_vil.cxx vidfpl_vil_menus.cxx vidfpl_vil_menus.h)
 ADD_EXECUTABLE (vidfpl_vil ${vidfpl_vil_sources})
 TARGET_LINK_LIBRARIES(vidfpl_vil vvid vidl1 vgui)

 # indirectly depends on vidl_vil1, through vvid_command_line_player.cxx
 SET(cl_grid_finder_sources command_line_grid_finder.cxx)
 ADD_EXECUTABLE (cl_grid_finder ${cl_grid_finder_sources})
 TARGET_LINK_LIBRARIES(cl_grid_finder sdet vpro vvid vul)
ENDIF (BUILD_UNMAINTAINED_LIBRARIES OR BUILD_FOR_VXL_DASHBOARD)

IF(HAS_CMU1394CAMERA)
 SET(vidster_sources vidster.cxx vidster_menus.cxx vidster_menus.h)
 ADD_EXECUTABLE (vidster ${vidster_sources})
 TARGET_LINK_LIBRARIES(vidster vvid vgui)

 IF (BUILD_UNMAINTAINED_LIBRARIES OR BUILD_FOR_VXL_DASHBOARD)
  # depending on vidl_vil1
  SET(videx_sources videx.cxx videx_menus.cxx videx_menus.h)
  ADD_EXECUTABLE (videx ${videx_sources})
  TARGET_LINK_LIBRARIES(videx vvid vidl_vil1 vgui)
 ENDIF (BUILD_UNMAINTAINED_LIBRARIES OR BUILD_FOR_VXL_DASHBOARD)
ENDIF(HAS_CMU1394CAMERA)
