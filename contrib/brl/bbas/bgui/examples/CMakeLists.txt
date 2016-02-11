include( ${VXL_CMAKE_DIR}/UseVGUI.cmake )

if( HAS_VGUI )
 add_executable( basic01_pick_tableau basic01_pick_tableau.cxx)
 target_link_libraries(basic01_pick_tableau bgui ${VXL_LIB_PREFIX}vgui)

 add_executable( basic02_graph_tableau basic02_graph_tableau.cxx)
 target_link_libraries(basic02_graph_tableau bgui ${VXL_LIB_PREFIX}vgui)

 add_executable( basic03_image_tableau basic03_image_tableau.cxx)
 target_link_libraries(basic03_image_tableau bgui ${VXL_LIB_PREFIX}vgui ${VXL_LIB_PREFIX}vil)
endif()
