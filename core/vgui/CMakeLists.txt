# oxl/vgui/CMakeLists.txt

INCLUDE(${MODULE_PATH}/NewCMake/FindOpenGL.cmake)

# Build here only if OpenGL is available:
IF (OPENGL_GLU_FOUND)

  INCLUDE(${MODULE_PATH}/NewCMake/FindX11.cmake)
  # Qv needed by vrml only
  INCLUDE(${MODULE_PATH}/FindQv.cmake)
  # glut optionally used by vgui_text and by the glut toolkit
  # implementation
  INCLUDE(${MODULE_PATH}/NewCMake/FindGLUT.cmake)

  # The following are for the optional toolkit
  # implementations
  INCLUDE(${MODULE_PATH}/NewCMake/FindQt.cmake)
  INCLUDE(${MODULE_PATH}/NewCMake/FindGTK.cmake)
  INCLUDE(${MODULE_PATH}/NewCMake/FindFLTK.cmake)
  INCLUDE(${MODULE_PATH}/NewCMake/FindMFC.cmake)

  ADD_DEFINITIONS( -DHAS_OPENGL)
  INCLUDE_DIRECTORIES(${OPENGL_INCLUDE_DIR})
  IF( GLUT_FOUND )
    INCLUDE_DIRECTORIES(${GLUT_INCLUDE_DIR})
    ADD_DEFINITIONS( -DHAS_GLUT)
  ENDIF( GLUT_FOUND )

  IF( OPENGL_XMESA_FOUND )
    ADD_DEFINITIONS( -DVGUI_MESA=1 )
  ELSE( OPENGL_XMESA_FOUND )
    ADD_DEFINITIONS( -DVGUI_MESA=0 )
  ENDIF( OPENGL_XMESA_FOUND )

SET(vgui_sources
  vgui_fwd.h

  # basic classes
  vgui.cxx                      vgui.h
  vgui_toolkit.cxx              vgui_toolkit.h
  vgui_tag.cxx                  vgui_tag.h
  vgui_window.cxx               vgui_window.h
  vgui_adaptor.cxx              vgui_adaptor.h
  vgui_tableau.cxx              vgui_tableau.h          vgui_tableau_sptr.h
  vgui_slot.cxx                 vgui_slot.h
  vgui_key.cxx                  vgui_key.h
  vgui_button.cxx               vgui_button.h
  vgui_modifier.cxx             vgui_modifier.h
  vgui_event.cxx                vgui_event.h
  vgui_command.cxx              vgui_command.h
  vgui_menu.cxx                 vgui_menu.h
  vgui_popup_params.cxx         vgui_popup_params.h
  vgui_message.cxx              vgui_message.h
  vgui_observer.cxx             vgui_observer.h
  vgui_observable.cxx           vgui_observable.h
  vgui_satellite.txx            vgui_satellite.h

  # utility
  vgui_find.cxx                 vgui_find.h
  vgui_utils.cxx                vgui_utils.h
  vgui_macro.cxx                vgui_macro.h
  vgui_color.cxx                vgui_color.h
  vgui_projection_inspector.cxx vgui_projection_inspector.h
  vgui_matrix_state.cxx         vgui_matrix_state.h
  vgui_text_graph.cxx           vgui_text_graph.h
  vgui_camera.cxx               vgui_camera.h
  vgui_event_condition.cxx      vgui_event_condition.h

  # images
  internals/vgui_rasterpos.cxx  internals/vgui_rasterpos.h
  vgui_pixel.cxx                vgui_pixel.h
  vgui_section_render.cxx       vgui_section_render.h
  vgui_section_buffer.cxx       vgui_section_buffer.h
  vgui_section_buffer_of.cxx    vgui_section_buffer_of.h
  vgui_texture_hacks.cxx        vgui_texture_hacks.h
  vgui_cache_wizard.cxx         vgui_cache_wizard.h
  vgui_image_renderer.cxx       vgui_image_renderer.h
  vgui_image_tableau.cxx        vgui_image_tableau.h    vgui_image_tableau_sptr.h
  vgui_image_blender.cxx        vgui_image_blender.h

  # tableau mixins and miscellaneous
  vgui_wrapper_tableau.cxx      vgui_wrapper_tableau.h  vgui_wrapper_tableau_sptr.h
  vgui_deck_tableau.cxx         vgui_deck_tableau.h     vgui_deck_tableau_sptr.h
  vgui_composite.cxx            vgui_composite.h        vgui_composite_sptr.h
  vgui_polytab.cxx              vgui_polytab.h          vgui_polytab_sptr.h
  vgui_grid_tableau.cxx         vgui_grid_tableau.h     vgui_grid_tableau_sptr.h
  vgui_drag_mixin.cxx           vgui_drag_mixin.h
  vgui_active_visible.cxx       vgui_active_visible.h   vgui_active_visible_sptr.h
  vgui_drag_tableau.cxx         vgui_drag_tableau.h
  vgui_clear_tableau.cxx        vgui_clear_tableau.h    vgui_clear_tableau_sptr.h
  vgui_debug_tableau.cxx        vgui_debug_tableau.h    vgui_debug_tableau_sptr.h
  vgui_quit_tableau.cxx         vgui_quit_tableau.h     vgui_quit_tableau_sptr.h
  vgui_tview_launcher.cxx       vgui_tview_launcher.h   vgui_tview_launcher_sptr.h
  vgui_shell_tableau.cxx        vgui_shell_tableau.h    vgui_shell_tableau_sptr.h
  vgui_text_put.cxx             vgui_text_put.h
  vgui_text_tableau.cxx         vgui_text_tableau.h     vgui_text_tableau_sptr.h
  vgui_blackbox.cxx             vgui_blackbox.h         vgui_blackbox_sptr.h
  vgui_enhance.cxx              vgui_enhance.h          vgui_enhance_sptr.h
  vgui_tview.cxx                vgui_tview.h            vgui_tview_sptr.h
  vgui_function_tableau.cxx     vgui_function_tableau.h
  vgui_test.cxx                 vgui_test.h             vgui_test_sptr.h
  vgui_roi_tableau.cxx          vgui_roi_tableau.h      vgui_roi_tableau_sptr.h
  vgui_rubberbander.cxx         vgui_rubberbander.h     vgui_rubberbander_sptr.h
  vgui_load.cxx                 vgui_load.h             vgui_load_sptr.h
  vgui_color_text.cxx           vgui_color_text.h
  vgui_event_server.cxx         vgui_event_server.h
  vgui_error_dialog.cxx         vgui_error_dialog.h

  # pcp
  vgui_soview.cxx               vgui_soview.h
  vgui_soview2D.cxx             vgui_soview2D.h
  vgui_soview3D.cxx             vgui_soview3D.h
  vgui_style.cxx                vgui_style.h
  vgui_style_factory.cxx        vgui_style_factory.h
  vgui_displaybase.cxx          vgui_displaybase.h      vgui_displaybase_sptr.h
  vgui_displaylist2D.cxx        vgui_displaylist2D.h    vgui_displaylist2D_sptr.h
  vgui_displaylist3D.cxx        vgui_displaylist3D.h
  vgui_easy2D.cxx               vgui_easy2D.h           vgui_easy2D_sptr.h
  vgui_easy3D.cxx               vgui_easy3D.h           vgui_easy3D_sptr.h
  vgui_listmanager2D.cxx        vgui_listmanager2D.h    vgui_listmanager2D_sptr.h
  vgui_viewer2D.cxx             vgui_viewer2D.h         vgui_viewer2D_sptr.h
  internals/trackball.c         internals/trackball.h
  vgui_viewer3D.cxx             vgui_viewer3D.h         vgui_viewer3D_sptr.h

  # dialog classes
  vgui_dialog.cxx                    vgui_dialog.h
  internals/vgui_dialog_field.cxx    internals/vgui_dialog_field.h
  internals/vgui_simple_field.cxx    internals/vgui_simple_field.h
  internals/vgui_string_field.cxx    internals/vgui_string_field.h
  internals/vgui_file_field.cxx      internals/vgui_file_field.h
  internals/vgui_dialog_impl.cxx     internals/vgui_dialog_impl.h

  # statusbar classes
  vgui_statusbuf.cxx            vgui_statusbuf.h
  vgui_statusbar.cxx            vgui_statusbar.h

  # experiment with new event model
  vgui_slab.cxx                 vgui_slab.h
  vgui_event_loop.cxx           vgui_event_loop.h

  # extra header files
  vgui_deck.h
  vgui_gl.h
  vgui_glu.h
  vgui_glx.h
  vgui_linker_hack.h

  # internals
  vgui_glut.cxx                      vgui_glut.h
  internals/vgui_invert_homg4x4.cxx  internals/vgui_invert_homg4x4.h
  internals/vgui_back_project.cxx    internals/vgui_back_project.h
  internals/vgui_un_project.cxx      internals/vgui_un_project.h
  internals/vgui_adaptor_tableau.cxx internals/vgui_adaptor_tableau.h
  internals/vgui_slot_data.cxx       internals/vgui_slot_data.h
  internals/vgui_adaptor_mixin.cxx   internals/vgui_adaptor_mixin.h
  internals/vgui_overlay_helper.cxx  internals/vgui_overlay_helper.h
  internals/vgui_multiply_4x4.cxx    internals/vgui_multiply_4x4.h
  internals/vgui_transpose_4x4.cxx   internals/vgui_transpose_4x4.h
  internals/vgui_draw_line.cxx       internals/vgui_draw_line.h
  # Mesa-X11-mfc accelerated functions using Hermes
  internals/vgui_accelerate.cxx      internals/vgui_accelerate.h
  internals/vgui_accelerate_tag.cxx
)

AUX_SOURCE_DIRECTORY(Templates vgui_sources)

IF (QV_FOUND)
  INCLUDE_DIRECTORIES(${QV_INCLUDE_DIR})
  SET(vgui_sources ${vgui_sources}
    vrml/vgui_vrml_draw_visitor.cxx  vrml/vgui_vrml_draw_visitor.h
    vrml/vgui_vrml_tableau.cxx       vrml/vgui_vrml_tableau.h
    vrml/vgui_vrml_texture_map.cxx   vrml/vgui_vrml_texture_map.h
  )
  AUX_SOURCE_DIRECTORY(vrml/Templates vgui_sources)
ENDIF (QV_FOUND)

IF (MFC_FOUND)
  ADD_DEFINITIONS(-DHAS_MFC)
  SET(vgui_sources ${vgui_sources}
    internals/vgui_accelerate_mfc.cxx internals/vgui_accelerate_mfc.h
  )
ENDIF (MFC_FOUND)

IF (X11_FOUND)
  INCLUDE_DIRECTORIES(${X11_INCLUDE_DIR})
  SET(vgui_sources ${vgui_sources}
    internals/vgui_accelerate_x11.cxx internals/vgui_accelerate_x11.h
  )
ENDIF (X11_FOUND)

IF (MFC_FOUND)
  # add stuff to use MFC in this executable
  ADD_DEFINITIONS( "-D_AFXDLL" )
  SET(CMAKE_MFC_FLAG 6)
ENDIF (MFC_FOUND)



######################################################################
# TOOLKIT IMPLEMENTATIONS
######################################################################



##################################################
# glut
##################################################

# Build here only if GLUT is available:
IF (GLUT_FOUND)

  OPTION(VGUI_USE_GLUT "Should Glut support be compiled into vgui?" YES)

  IF(VGUI_USE_GLUT)

    ADD_DEFINITIONS( "-DVGUI_USE_GLUT" )

    INCLUDE_DIRECTORIES(${GLUT_INCLUDE_DIR})

    SET(vgui_glut_sources
      impl/glut/vgui_glut_adaptor.cxx    impl/glut/vgui_glut_adaptor.h
      impl/glut/vgui_glut_tag.cxx
      impl/glut/vgui_glut_popup_impl.cxx impl/glut/vgui_glut_popup_impl.h
      impl/glut/vgui_glut_window.cxx     impl/glut/vgui_glut_window.h
      impl/glut/vgui_glut_impl.cxx       impl/glut/vgui_glut_impl.h
      impl/glut/vgui_glut_slab.cxx       impl/glut/vgui_glut_slab.h

      impl/glut/menu_hack.h
    )
    IF (X11_FOUND)
      INCLUDE_DIRECTORIES(${X11_INCLUDE_DIR})
      SET(vgui_glut_sources ${vgui_glut_sources}
        impl/glut/menu_hack_X11.cxx
      )
    ELSE (X11_FOUND)
      SET(vgui_glut_sources ${vgui_glut_sources}
        impl/glut/menu_hack_none.cxx
      )
    ENDIF (X11_FOUND)

    SET( vgui_sources ${vgui_sources} ${vgui_glut_sources} )

    AUX_SOURCE_DIRECTORY(impl/glut/Templates vgui_sources)

  ENDIF(VGUI_USE_GLUT)

ENDIF (GLUT_FOUND)


##################################################
# Qt
##################################################

IF (QT_FOUND)
IF (QT_WRAP_CPP)

  OPTION(VGUI_USE_QT "Should Qt support be compiled into vgui?" YES)

  IF(VGUI_USE_QT)

    ADD_DEFINITIONS( ${QT_DEFINITIONS} "-DVGUI_USE_QT" )

    INCLUDE_DIRECTORIES(${QT_INCLUDE_DIR})
    ADD_DEFINITIONS(${QT_DEFINITIONS})

    SET(vgui_qt_sources
      impl/qt/vgui_qt.cxx             impl/qt/vgui_qt.h
      impl/qt/vgui_qt_tag.cxx
      impl/qt/vgui_qt_window.cxx      impl/qt/vgui_qt_window.h
      impl/qt/vgui_qt_adaptor.cxx     impl/qt/vgui_qt_adaptor.h
      impl/qt/vgui_qt_statusbar.cxx   impl/qt/vgui_qt_statusbar.h
      impl/qt/vgui_qt_dialog_impl.cxx impl/qt/vgui_qt_dialog_impl.h
      impl/qt/vgui_qt_menu.cxx        impl/qt/vgui_qt_menu.h
    )

    QT_WRAP_CPP(vgui vgui_qt_moc_files
      impl/qt/vgui_qt_adaptor.h
      impl/qt/vgui_qt_statusbar.h
      impl/qt/vgui_qt_dialog_impl.h
      impl/qt/vgui_qt_menu.h
    )

    SET( vgui_sources ${vgui_sources} ${vgui_qt_sources} ${vgui_qt_moc_files} )

  ENDIF(VGUI_USE_QT)

ENDIF (QT_WRAP_CPP)
ENDIF (QT_FOUND)


##################################################
# MFC
##################################################


IF (MFC_FOUND)

  OPTION(VGUI_USE_MFC "Should MFC support be compiled into vgui?" YES)

  IF(VGUI_USE_MFC)

    ADD_DEFINITIONS( "-DVGUI_USE_MFC" )

    SET(vgui_mfc_sources
      impl/mfc/vgui_mfc.cxx             impl/mfc/vgui_mfc.h
      impl/mfc/vgui_mfc_adaptor.cxx     impl/mfc/vgui_mfc_adaptor.h
      impl/mfc/vgui_mfc_app.cxx         impl/mfc/vgui_mfc_app.h
      impl/mfc/vgui_mfc_app_init.cxx    impl/mfc/vgui_mfc_app_init.h
      impl/mfc/vgui_mfc_dialog_impl.cxx impl/mfc/vgui_mfc_dialog_impl.h
      impl/mfc/vgui_mfc_doc.cxx         impl/mfc/vgui_mfc_doc.h
      impl/mfc/vgui_mfc_mainfrm.cxx     impl/mfc/vgui_mfc_mainfrm.h
      impl/mfc/vgui_mfc_statusbar.cxx   impl/mfc/vgui_mfc_statusbar.h
      impl/mfc/vgui_mfc_tag.cxx
      impl/mfc/vgui_mfc_util.cxx        impl/mfc/vgui_mfc_util.h
      impl/mfc/vgui_mfc_utils.cxx       impl/mfc/vgui_mfc_utils.h
      impl/mfc/vgui_mfc_view.cxx        impl/mfc/vgui_mfc_view.h
      impl/mfc/vgui_mfc_window.cxx      impl/mfc/vgui_mfc_window.h
    )

    SET( vgui_sources ${vgui_sources} ${vgui_mfc_sources} )

  ENDIF(VGUI_USE_MFC)

ENDIF (MFC_FOUND)


##################################################
# X
##################################################

IF (X11_FOUND)

  OPTION(VGUI_USE_X "Should X support be compiled into vgui?" YES)

  IF(VGUI_USE_X)

    ADD_DEFINITIONS( "-DVGUI_USE_X" )

    INCLUDE_DIRECTORIES(${X11_INCLUDE_DIR})

    SET(vgui_X_sources
      impl/X/vgui_X_utils.cxx impl/X/vgui_X_utils.h
    )

    SET( vgui_sources ${vgui_sources} ${vgui_X_sources} )

  ENDIF(VGUI_USE_X)

ENDIF (X11_FOUND)


##################################################
# glX
##################################################

IF (HAS_GLX)

  OPTION(VGUI_USE_GLX "Should glX support be compiled into vgui?" YES)

  IF(VGUI_USE_GLX)

    ADD_DEFINITIONS( "-DVGUI_USE_GLX" )

    SET(vgui_IMPL_GLX "vgui_glX" CACHE INTERNAL "Set if vgui has GLX")

    SET(vgui_glX_sources
      impl/glX/vgui_glX_window.cxx  impl/glX/vgui_glX_window.h
      impl/glX/vgui_glX_tag.cxx
      impl/glX/vgui_glX_adaptor.cxx impl/glX/vgui_glX_adaptor.h
      impl/glX/vgui_glX.cxx         impl/glX/vgui_glX.h
    )

    SET( vgui_sources ${vgui_sources} ${vgui_glX_sources} )

    AUX_SOURCE_DIRECTORY(impl/glX/Templates vgui_glX_sources)

  ENDIF(VGUI_USE_GLX)

ENDIF (HAS_GLX)


##################################################
# GTK
##################################################

IF (GTK_FOUND)

  OPTION(VGUI_USE_GTK "Should GTK support be compiled into vgui?" YES)

  IF(VGUI_USE_GTK)

    ADD_DEFINITIONS( "-DVGUI_USE_GTK" )

    INCLUDE_DIRECTORIES(${GTK_INCLUDE_DIR})

    SET(vgui_gtk_sources
      impl/gtk/vgui_gtk.cxx             impl/gtk/vgui_gtk.h
      impl/gtk/vgui_gtk_adaptor.cxx     impl/gtk/vgui_gtk_adaptor.h
      impl/gtk/vgui_gtk_dialog_impl.cxx impl/gtk/vgui_gtk_dialog_impl.h
      impl/gtk/vgui_gtk_statusbar.cxx   impl/gtk/vgui_gtk_statusbar.h
      impl/gtk/vgui_gtk_tag.cxx
      impl/gtk/vgui_gtk_utils.cxx       impl/gtk/vgui_gtk_utils.h
      impl/gtk/vgui_gtk_window.cxx      impl/gtk/vgui_gtk_window.h
    )

    SET( vgui_sources ${vgui_sources} ${vgui_gtk_sources} )

    AUX_SOURCE_DIRECTORY(impl/gtk/Templates vgui_gtk_sources)

  ENDIF(VGUI_USE_GTK)

ENDIF (GTK_FOUND)


##################################################
# SDL
##################################################

IF (HAS_SDL)

  OPTION(VGUI_USE_SDL "Should SDL support be compiled into vgui?" YES)

  IF(VGUI_USE_SDL)

    ADD_DEFINITIONS( "-DVGUI_USE_SDL" )

    SET(vgui_SDL_sources
      impl/SDL/vgui_SDL.cxx         impl/SDL/vgui_SDL.h
      impl/SDL/vgui_SDL_tag.cxx
      impl/SDL/vgui_SDL_factory.cxx impl/SDL/vgui_SDL_factory.h
      impl/SDL/vgui_SDL_window.cxx  impl/SDL/vgui_SDL_window.h
      impl/SDL/vgui_SDL_adaptor.cxx impl/SDL/vgui_SDL_adaptor.h
    )

    SET( vgui_sources ${vgui_sources} ${vgui_SDL_sources} )

    #ADD_EXECUTABLE(test-SDL test-SDL.cxx)
    #TARGET_LINK_LIBRARIES(test-SDL vgui_SDL)

  ENDIF(VGUI_USE_SDL)

ENDIF (HAS_SDL)


##################################################
# Motif
##################################################

IF (HAS_MOTIF)

  OPTION(VGUI_USE_XM "Should Motif support be compiled into vgui?" YES)

  IF(VGUI_USE_XM)

    ADD_DEFINITIONS( "-DVGUI_USE_XM" )

    SET(vgui_Xm_sources
      impl/Xm/vgui_Xm_window.cxx      impl/Xm/vgui_Xm_window.h
      impl/Xm/vgui_Xm_utils.cxx       impl/Xm/vgui_Xm_utils.h
      impl/Xm/vgui_Xm_tag.cxx
      impl/Xm/vgui_Xm_statusbar.cxx   impl/Xm/vgui_Xm_statusbar.h
      impl/Xm/vgui_Xm_dialog_impl.cxx impl/Xm/vgui_Xm_dialog_impl.h
      impl/Xm/vgui_Xm_adaptor.cxx     impl/Xm/vgui_Xm_adaptor.h
      impl/Xm/vgui_Xm.cxx             impl/Xm/vgui_Xm.h
    )

    SET( vgui_sources ${vgui_sources} ${vgui_Xm_sources} )

    AUX_SOURCE_DIRECTORY(impl/Xm/Templates vgui_Xm_sources)

  ENDIF(VGUI_USE_XM)

ENDIF (HAS_MOTIF)


##################################################
# FLTK
##################################################

IF (FLTK_FOUND)

  OPTION(VGUI_USE_FL "Should Fl support be compiled into vgui?" YES)

  IF(VGUI_USE_FL)

    ADD_DEFINITIONS( "-DVGUI_USE_FL" )

    INCLUDE_DIRECTORIES(${X11_INCLUDE_DIR} ${FLTK_INCLUDE_DIR})

    SET(vgui_Fl_sources
      impl/Fl/vgui_Fl.cxx             impl/Fl/vgui_Fl.h
      impl/Fl/vgui_Fl_tag.cxx
      impl/Fl/vgui_Fl_window.cxx      impl/Fl/vgui_Fl_window.h
      impl/Fl/vgui_Fl_adaptor.cxx     impl/Fl/vgui_Fl_adaptor.h
      impl/Fl/vgui_Fl_dialog_impl.cxx impl/Fl/vgui_Fl_dialog_impl.h
      impl/Fl/vgui_Fl_utils.cxx       impl/Fl/vgui_Fl_utils.h
      impl/Fl/vgui_Fl_statusbar.cxx   impl/Fl/vgui_Fl_statusbar.h
    )

    SET( vgui_sources ${vgui_sources} ${vgui_Fl_sources} )

  ENDIF(VGUI_USE_FL)

ENDIF (FLTK_FOUND)


######################################################################
# END TOOLKIT IMPLEMENTATIONS
######################################################################


# This file uses the -DVGUI_USE_* preprocessor definitions set by
# the implementations to register each toolkit that was compiled
# in.

SET( vgui_sources ${vgui_sources}
  vgui_register_all.cxx
)


ADD_LIBRARY(vgui ${vgui_sources})

# Basic vgui dependencies

TARGET_LINK_LIBRARIES(vgui vnl_algo vnl vil vgl vpl vbl vul)
IF (GLUT_FOUND)
  TARGET_LINK_LIBRARIES(vgui ${GLUT_LIBRARIES})
ENDIF (GLUT_FOUND)
IF (QV_FOUND)
  TARGET_LINK_LIBRARIES(vgui ${QV_LIBRARIES})
ENDIF (QV_FOUND)

TARGET_LINK_LIBRARIES(vgui ${OPENGL_LIBRARIES})

IF (X11_FOUND)
  TARGET_LINK_LIBRARIES(vgui ${X11_LIBRARIES})
ENDIF (X11_FOUND)


# Dependencies created by the toolkit implementations

IF (VGUI_USE_GLUT)
  TARGET_LINK_LIBRARIES(vgui vil vul ${GLUT_LIBRARIES})
  IF (X11_FOUND)
    TARGET_LINK_LIBRARIES(vgui ${X11_LIBRARIES})
  ENDIF (X11_FOUND)
ENDIF (VGUI_USE_GLUT)
IF (VGUI_USE_QT)
  TARGET_LINK_LIBRARIES(vgui vul vbl ${QT_LIBRARIES})
ENDIF (VGUI_USE_QT)
IF (VGUI_USE_MFC)
  TARGET_LINK_LIBRARIES(vgui vul)
ENDIF (VGUI_USE_MFC)
IF (VGUI_USE_X)
  TARGET_LINK_LIBRARIES(vgui ${X11_LIBRARIES})
ENDIF (VGUI_USE_X)
IF (VGUI_USE_GLX)
  TARGET_LINK_LIBRARIES(vgui vul)
ENDIF (VGUI_USE_GLX)
IF (VGUI_USE_GTK)
  TARGET_LINK_LIBRARIES(vgui vul ${GTK_LIBRARIES})
ENDIF (VGUI_USE_GTK)
IF (VGUI_USE_SDL)
  TARGET_LINK_LIBRARIES(vgui vul)
ENDIF (VGUI_USE_SDL)
IF (VGUI_USE_XM)
  TARGET_LINK_LIBRARIES(vgui vul)
ENDIF (VGUI_USE_XM)
IF (VGUI_USE_FL)
  TARGET_LINK_LIBRARIES(vgui vul ${FLTK_LIBRARIES} ${X11_LIBRARIES})
ENDIF (VGUI_USE_FL)


IF (BUILD_EXAMPLES)
  SUBDIRS(examples)
ENDIF (BUILD_EXAMPLES)

IF (BUILD_TESTING)
  SUBDIRS(tests)
ENDIF (BUILD_TESTING)

ENDIF (OPENGL_GLU_FOUND)
