# gel/jvid/bin/CMakeLists.txt

ADD_EXECUTABLE( jplayer jplayer.cxx )

INCLUDE_DIRECTORIES( ${allvxl_SOURCE_DIR}/vxl )	

INCLUDE( ${allvxl_SOURCE_DIR}/gel/jvid/CMakeListsLink.txt )
INCLUDE( ${allvxl_SOURCE_DIR}/oxl/vgui/CMakeListsLink.txt )
INCLUDE( ${allvxl_SOURCE_DIR}/vxl/vidl/CMakeListsLink.txt )
INCLUDE( ${allvxl_SOURCE_DIR}/vcl/CMakeListsLink.txt )
