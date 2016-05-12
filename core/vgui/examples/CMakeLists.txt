include( ${VXL_CMAKE_DIR}/UseVGUI.cmake )

if( HAS_VGUI )

link_libraries( ${VXL_LIB_PREFIX}vgui )

add_executable( basic01_display_image basic01_display_image.cxx)
add_executable( basic01a_multiple_panes basic01a_multiple_panes.cxx)
add_executable( basic01b_deck basic01b_deck.cxx)
target_link_libraries( basic01b_deck ${VXL_LIB_PREFIX}vgui ${VXL_LIB_PREFIX}vnl )
add_executable( basic01c_display_vil_image basic01c_display_vil_image.cxx)
add_executable( basic01d_display_vil1_image basic01d_display_vil1_image.cxx)
add_executable( basic02_display_line basic02_display_line.cxx)
add_executable( basic03_display_text basic03_display_text.cxx)
add_executable( basic04_display_2d basic04_display_2d.cxx)
add_executable( basic05_menubar basic05_menubar.cxx)
add_executable( basic06_popup basic06_popup.cxx)
add_executable( basic07_dialog basic07_dialog.cxx)
add_executable( basic08_printing basic08_printing.cxx)
add_executable( basic09_mouse_position basic09_mouse_position.cxx)
add_executable( basic10_draw_line basic10_draw_line.cxx)
add_executable( basic10a_rubberband basic10a_rubberband.cxx)
add_executable( basic11_display_3d basic11_display_3d.cxx)
add_executable( example_display_pyramid example_display_pyramid.cxx)
add_executable( example_draw_polygon example_draw_polygon.cxx)
target_link_libraries( example_draw_polygon ${VXL_LIB_PREFIX}vgui ${VXL_LIB_PREFIX}vnl ${VXL_LIB_PREFIX}vgl )
add_executable( example_get_highlighted example_get_highlighted.cxx)
add_executable( example_multiple_windows example_multiple_windows.cxx)
add_executable( example_vgui_observer example_vgui_observer.cxx)
add_executable( example_satellite_tableau example_satellite_tableau.cxx)
add_executable( example_blender_tableau example_blender_tableau.cxx)
add_executable( example_enhance_tableau example_enhance_tableau.cxx)
add_executable( example_poly_tableau example_poly_tableau.cxx)
add_executable( example_text_windows example_text_windows.cxx)
add_executable( example_sprite example_sprite.cxx )
target_link_libraries( example_sprite ${VXL_LIB_PREFIX}vgui ${VXL_LIB_PREFIX}vil ${VXL_LIB_PREFIX}vul )
add_executable( example_idle_processing example_idle_processing.cxx )
add_executable( example_slider example_slider.cxx)
add_executable( click_points click_points.cxx)
add_executable( example_gllist_tableau example_gllist_tableau.cxx)


set(basic_sources
    basic_app.cxx
    basic_manager.cxx  basic_manager.h
    basic_menus.cxx    basic_menus.h)
add_executable(basic_app ${basic_sources})
target_link_libraries(basic_app ${VXL_LIB_PREFIX}vgui ${VXL_LIB_PREFIX}vul ${VXL_LIB_PREFIX}vbl)


# GTK examples
#-------------
# The examples use the gtk adaptor (not the gtk2 adaptor), so
# vgui must have been compiled with gtk1 support for these
# to work.  They also require gktgl.
find_package(GTK)
if(VGUI_USE_GTK)
if(GTK_GL_FOUND)
 include_directories( ${GTK_INCLUDE_DIR} )

 add_executable( gtk1_display_image gtk1_display_image.cxx)
 target_link_libraries( gtk1_display_image ${GTK_LIBRARIES} )

 add_executable( gtk2_movie_player gtk2_movie_player.cxx )
 target_link_libraries( gtk2_movie_player ${GTK_LIBRARIES} )

 add_executable( gtk3_vidl gtk3_vidl.cxx)
 target_link_libraries( gtk3_vidl ${GTK_LIBRARIES} )
endif()
endif()

# GLUT examples
#--------------
find_package(GLUT)
if(GLUT_FOUND)
 target_link_libraries( click_points ${GLUT_LIBRARIES} )

 add_executable( glut1_raw glut1_raw.cxx )
 target_link_libraries( glut1_raw ${GLUT_LIBRARIES} )

 add_executable( glut2_raw glut2_raw.cxx )
 target_link_libraries( glut2_raw ${GLUT_LIBRARIES} )
endif()

# MFC example
#------------
if(NOT MINGW)
  # MFC is not available in MinGW.
  # This should be fixed in FindMFC.cmake and then this IF can be removed
  find_package(MFC)
endif()
if(VGUI_USE_MFC)
   add_subdirectory(mfc_example)
endif()

# wxWidgets example
#------------------
if(VGUI_USE_WX)
  add_subdirectory(wx_example)
endif()

endif()
