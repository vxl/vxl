# oxl/VPLAYER/CMakeLists.txt

INCLUDE( ${MODULE_PATH}/UseVGUI.cmake )

IF (HAS_VGUI)
 IF (WIN32)
  IF (NOT CYGWIN)
    SET(VPLAYER_sources
     easy2D_loader.cxx    easy2D_loader.h
     easy2D_sequencer.cxx easy2D_sequencer.h
     vplayer.cxx          vplayer.h
     vplayer_file.cxx     vplayer_file.h
     vplayer_geometry.cxx vplayer_geometry.h
     vplayer_video.cxx    vplayer_video.h
    )

    ADD_EXECUTABLE(VPLAYER ${VPLAYER_sources})
    TARGET_LINK_LIBRARIES(VPLAYER vfw32 osl mvl vgui vidl_vil1 oxp)
  ENDIF (NOT CYGWIN)
 ENDIF (WIN32)
ENDIF (HAS_VGUI)
