INCLUDE( ${VXL_CMAKE_DIR}/UseVGUI.cmake )

IF( HAS_VGUI )

LINK_LIBRARIES( vgui )

ADD_EXECUTABLE( basic01_display_image basic01_display_image.cxx)
ADD_EXECUTABLE( basic01a_multiple_panes basic01a_multiple_panes.cxx)
ADD_EXECUTABLE( basic01b_deck basic01b_deck.cxx)
ADD_EXECUTABLE( basic01c_display_vil_image basic01c_display_vil_image.cxx)
ADD_EXECUTABLE( basic01d_display_vil1_image basic01d_display_vil1_image.cxx)
ADD_EXECUTABLE( basic02_display_line basic02_display_line.cxx)
ADD_EXECUTABLE( basic03_display_text basic03_display_text.cxx)
ADD_EXECUTABLE( basic04_display_2d basic04_display_2d.cxx)
ADD_EXECUTABLE( basic05_menubar basic05_menubar.cxx)
ADD_EXECUTABLE( basic06_popup basic06_popup.cxx)
ADD_EXECUTABLE( basic07_dialog basic07_dialog.cxx)
ADD_EXECUTABLE( basic08_printing basic08_printing.cxx)
ADD_EXECUTABLE( basic09_mouse_position basic09_mouse_position.cxx)
ADD_EXECUTABLE( basic10_draw_line basic10_draw_line.cxx)
ADD_EXECUTABLE( basic10a_rubberband basic10a_rubberband.cxx)
ADD_EXECUTABLE( basic11_display_3d basic11_display_3d.cxx)
ADD_EXECUTABLE( example_display_pyramid example_display_pyramid.cxx)
ADD_EXECUTABLE( example_draw_polygon example_draw_polygon.cxx)
ADD_EXECUTABLE( example_get_highlighted example_get_highlighted.cxx)
ADD_EXECUTABLE( example_multiple_windows example_multiple_windows.cxx)
ADD_EXECUTABLE( example_vgui_observer example_vgui_observer.cxx)
ADD_EXECUTABLE( example_satellite_tableau example_satellite_tableau.cxx)
ADD_EXECUTABLE( example_blender_tableau example_blender_tableau.cxx)
ADD_EXECUTABLE( example_enhance_tableau example_enhance_tableau.cxx)
ADD_EXECUTABLE( example_poly_tableau example_poly_tableau.cxx)
ADD_EXECUTABLE( example_sprite example_sprite.cxx )
TARGET_LINK_LIBRARIES( example_sprite vgui vil vul )

SET(basic_sources
    basic_app.cxx
    basic_manager.cxx  basic_manager.h
    basic_menus.cxx    basic_menus.h)
ADD_EXECUTABLE(basic_app ${basic_sources})
TARGET_LINK_LIBRARIES(basic_app vgui vul vbl)


# GTK examples
#-------------
# The examples use the gtk adaptor (not the gtk2 adaptor), so
# vgui must have been compiled with gtk1 support for these
# to work.
INCLUDE(${CMAKE_ROOT}/Modules/FindGTK.cmake)
IF(VGUI_USE_GTK)
 INCLUDE_DIRECTORIES( ${GTK_INCLUDE_DIR} )

 ADD_EXECUTABLE( gtk1_display_image gtk1_display_image.cxx)
 TARGET_LINK_LIBRARIES( gtk1_display_image ${GTK_LIBRARIES} )

 ADD_EXECUTABLE( gtk2_movie_player gtk2_movie_player.cxx )
 TARGET_LINK_LIBRARIES( gtk2_movie_player ${GTK_LIBRARIES} )

 ADD_EXECUTABLE( gtk3_vidl gtk3_vidl.cxx)
 TARGET_LINK_LIBRARIES( gtk3_vidl vidl_vil1 ${GTK_LIBRARIES} )
ENDIF(VGUI_USE_GTK)

# GLUT examples
#--------------
INCLUDE( ${CMAKE_ROOT}/Modules/FindGLUT.cmake )
IF(GLUT_FOUND)
 ADD_EXECUTABLE( glut1_raw glut1_raw.cxx )
 TARGET_LINK_LIBRARIES( glut1_raw ${GLUT_LIBRARIES} )

 ADD_EXECUTABLE( glut2_raw glut2_raw.cxx )
 TARGET_LINK_LIBRARIES( glut2_raw ${GLUT_LIBRARIES} )
ENDIF (GLUT_FOUND)

# MFC example
#------------
INCLUDE(${CMAKE_ROOT}/Modules/FindMFC.cmake)
IF(MFC_FOUND)
   SUBDIRS( mfc_example )
ENDIF(MFC_FOUND)

ENDIF( HAS_VGUI )
