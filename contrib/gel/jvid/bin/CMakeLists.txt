# ./gel/jvid/bin/CMakeLists.txt
INCLUDE(${CMAKE_SOURCE_DIR}/config.cmake/Modules/FindOpenGL.cmake)
ADD_EXECUTABLE (jplayer jplayer.cxx)

INCLUDE_DIRECTORIES( ${allvxl_SOURCE_DIR}/vxl )	


 INCLUDE( ${allvxl_SOURCE_DIR}/oxl/osl/CMakeListsLink.txt )
 INCLUDE( ${allvxl_SOURCE_DIR}/oxl/mvl/CMakeListsLink.txt )
 INCLUDE( ${allvxl_SOURCE_DIR}/oxl/vgui/CMakeListsLink.txt )
 INCLUDE( ${allvxl_SOURCE_DIR}/vxl/vidl/CMakeListsLink.txt )
 INCLUDE( ${allvxl_SOURCE_DIR}/oxl/oxp/CMakeListsLink.txt )
 INCLUDE( ${allvxl_SOURCE_DIR}/gel/jvid/CMakeListsLink.txt )
