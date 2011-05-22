# vxl/vidl1/examples/CMakeLists.txt
INCLUDE( ${VXL_CMAKE_DIR}/UseVGUI.cmake )


ADD_EXECUTABLE( vidl1_mpegcodec_example vidl1_mpegcodec_example.cxx )
TARGET_LINK_LIBRARIES( vidl1_mpegcodec_example vidl1 )

ADD_EXECUTABLE( vidl1_video2images vidl1_video2images.cxx )
TARGET_LINK_LIBRARIES( vidl1_video2images vidl1 vil vul vcl )


IF( HAS_VGUI )

INCLUDE_DIRECTORIES( ${VXLCORE_INCLUDE_DIR} )

SET(vidl1_player_sources
    vidl1_player_main.cxx
    vidl1_player_menus.cxx      vidl1_player_menus.h
    vidl1_player_manager.cxx    vidl1_player_manager.h
   )

ADD_EXECUTABLE (vidl1_player ${vidl1_player_sources})
TARGET_LINK_LIBRARIES(vidl1_player vidl1 vgui)

ENDIF( HAS_VGUI )
