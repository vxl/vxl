# ./brl/vvid/bin/CMakeLists.txt


INCLUDE_DIRECTORIES( ${allvxl_SOURCE_DIR}/vxl )	

SOURCE_FILES(vidfpl_sources vidfpl_menus) 
SOURCE_FILES(videx_sources videx_menus) 

ADD_EXECUTABLE (vidfpl vidfpl.cxx vidfpl_sources)
ADD_EXECUTABLE (videx videx.cxx videx_sources)

TARGET_LINK_LIBRARIES(videx osl mvl oxp vvid vidl vgui )
TARGET_LINK_LIBRARIES(vidfpl osl mvl oxp vvid vidl vgui )
