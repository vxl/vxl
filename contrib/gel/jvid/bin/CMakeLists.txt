# ./gel/jvid/bin/CMakeLists.txt


INCLUDE_DIRECTORIES( ${allvxl_SOURCE_DIR}/vxl )	

ADD_EXECUTABLE (jplayer jplayer.cxx)

TARGET_LINK_LIBRARIES(jplayer osl mvl jvid vidl vgui )
