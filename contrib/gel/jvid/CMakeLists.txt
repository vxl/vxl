# ./gel/jvid/CMakeLists.txt

INCLUDE( ${allvxl_SOURCE_DIR}/oxl/vgui/UseVGUI.cmake )
 
IF(HAS_VGUI)

  INCLUDE_DIRECTORIES( ${allvxl_SOURCE_DIR}/oxl )

  INCLUDE(${MODULE_PATH}/NewCMake/FindMPEG.cmake)  
  IF(MPEG_FOUND)
    ADD_DEFINITIONS(-DHAS_MPEG)
  ENDIF(MPEG_FOUND)

  # Uses the vidl_avicodec, and thus will only build under Windows
  IF(WIN32)
    IF (NOT CYGWIN)

      SET( jvid_sources
        jvx_manager.cxx jvx_manager.h
      )

      AUX_SOURCE_DIRECTORY(Templates jvid_sources)

      ADD_LIBRARY(jvid ${jvid_sources})
      TARGET_LINK_LIBRARIES( jvid vgui vidl vul vil )

      SUBDIRS(bin)

    ENDIF (NOT CYGWIN)
  ENDIF(WIN32)
ENDIF(HAS_VGUI)
