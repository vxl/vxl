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
TARGET_LINK_LIBRARIES(vidl2_player vidl2 vgui vil vul)

ENDIF( HAS_VGUI )
