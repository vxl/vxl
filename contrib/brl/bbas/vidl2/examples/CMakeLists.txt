# contrib/brl/bbas/vidl2/examples/CMakeLists.txt
INCLUDE( ${MODULE_PATH}/UseVGUI.cmake )

IF( HAS_VGUI )

INCLUDE_DIRECTORIES( ${VXLCORE_INCLUDE_DIR} )

SET(vidl2_player_sources
    vidl2_player_main.cxx
    vidl2_player_menus.cxx      vidl2_player_menus.h
    vidl2_player_manager.cxx    vidl2_player_manager.h
)

ADD_EXECUTABLE (vidl2_player ${vidl2_player_sources})
TARGET_LINK_LIBRARIES(vidl2_player vidl2 vidl2_gui vul)

ENDIF( HAS_VGUI )


IF( DIRECTSHOW_FOUND )

# An example use of vidl2_dshow_file_istream
ADD_EXECUTABLE (vidl2_dshow_file_example vidl2_dshow_file_example.cxx)
TARGET_LINK_LIBRARIES(vidl2_dshow_file_example vidl2)

# An example use of vidl2_dshow_live_istream
INCLUDE_DIRECTORIES( ${VXL_MUL_INCLUDE_DIR} )
ADD_EXECUTABLE (vidl2_dshow_live_example vidl2_dshow_live_example.cxx)
TARGET_LINK_LIBRARIES(vidl2_dshow_live_example vidl2 mbl)

ENDIF( DIRECTSHOW_FOUND )

IF ( VIDEODEV_FOUND )
   ADD_EXECUTABLE (vidl2_v4l_example  vidl2_v4l_live_example.cxx)
   TARGET_LINK_LIBRARIES(vidl2_v4l_example vidl2)
ENDIF ( VIDEODEV_FOUND )
