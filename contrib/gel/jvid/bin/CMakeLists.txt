# ./gel/jvid/bin/CMakeLists.txt


INCLUDE_DIRECTORIES( ${VXLCORE_INCLUDE_DIR} )

ADD_EXECUTABLE (jplayer jplayer.cxx)

TARGET_LINK_LIBRARIES(jplayer osl mvl jvid vidl_vil1 vgui )
