INCLUDE( ${MODULE_PATH}/UseVGUI.cmake )

IF( HAS_VGUI )
 ADD_EXECUTABLE( basic01_pick_tableau basic01_pick_tableau.cxx)
 TARGET_LINK_LIBRARIES(basic01_pick_tableau bgui vgui)

 ADD_EXECUTABLE( basic02_graph_tableau basic02_graph_tableau.cxx)
 TARGET_LINK_LIBRARIES(basic02_graph_tableau bgui vgui)
ENDIF( HAS_VGUI )
