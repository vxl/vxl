# ./gel/jvid/CMakeLists.txt

INCLUDE( ${VXL_CMAKE_DIR}/UseVGUI.cmake )
 
IF(HAS_VGUI)

  INCLUDE_DIRECTORIES( ${OXL_INCLUDE_DIR} )

  INCLUDE(${MODULE_PATH}/FindMPEG2.cmake)  
  IF(MPEG2_FOUND)
    ADD_DEFINITIONS(-DHAS_MPEG2)
    INCLUDE_DIRECTORIES(${MPEG2_INCLUDE_DIR})
  ENDIF(MPEG2_FOUND)

  SET( jvid_sources
    jvx_manager.cxx jvx_manager.h
  )

  AUX_SOURCE_DIRECTORY(Templates jvid_sources)

  ADD_LIBRARY(jvid ${jvid_sources})
  TARGET_LINK_LIBRARIES( jvid vgui vidl vul vil1 )
  IF(MPEG2_FOUND)
    TARGET_LINK_LIBRARIES( jvid ${MPEG2_LIBRARIES} )
  ENDIF(MPEG2_FOUND)

  SUBDIRS(bin)

ENDIF(HAS_VGUI)
