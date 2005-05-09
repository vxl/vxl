INCLUDE( ${MODULE_PATH}/UseVGUI.cmake )

IF( HAS_VGUI )

LINK_LIBRARIES( bgui )

ADD_EXECUTABLE( basic01_pick_tableau basic01_pick_tableau.cxx)

ENDIF( HAS_VGUI )
