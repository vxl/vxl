# brl/vvid/bin/CMakeLists.txt

INCLUDE_DIRECTORIES( ${VXLCORE_INCLUDE_DIR} )

SET(vidfpl_sources vidfpl.cxx vidfpl_menus.cxx vidfpl_menus.h)
ADD_EXECUTABLE (vidfpl ${vidfpl_sources})
TARGET_LINK_LIBRARIES(vidfpl osl mvl oxp vvid vidl vgui )

IF(HAS_CMU1394CAMERA)
 SET(videx_sources videx.cxx videx_menus.cxx videx_menus.h)
 ADD_EXECUTABLE (videx ${videx_sources})
 TARGET_LINK_LIBRARIES(videx osl mvl oxp vvid vidl vgui )
ENDIF(HAS_CMU1394CAMERA)
