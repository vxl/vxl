# oxl/vgui/CMakeLists.txt

# Default the dashboard builds to "YES" so that
# we have some clients that try to compile vgui
IF( DART_ROOT )
  OPTION( BUILD_VGUI "Build VGUI" "YES" )
ELSE( DART_ROOT )
  OPTION( BUILD_VGUI "Build VGUI" "NO" )
ENDIF( DART_ROOT )

IF( BUILD_VGUI )

INCLUDE(${MODULE_PATH}/NewCMake/FindOpenGL.cmake)

# Build here only if OpenGL is available:
IF (OPENGL_FOUND)
 IF (OPENGL_GLU_FOUND)

  # For (not) reporting an error below
  SET( CAN_BUILD_VGUI "YES" )

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
  INCLUDE(${MODULE_PATH}/FindGTK2.cmake)
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
  vgui_parent_child_link.cxx    vgui_parent_child_link.h
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
  vgui_satellite_tableau.txx vgui_satellite_tableau.h

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
  vgui_texture_hacks.cxx        vgui_texture_hacks.h
  vgui_cache_wizard.cxx         vgui_cache_wizard.h
  vgui_image_renderer.cxx       vgui_image_renderer.h
  vgui_vil_image_renderer.cxx   vgui_vil_image_renderer.h
  vgui_image_tableau.cxx        vgui_image_tableau.h      vgui_image_tableau_sptr.h
  vgui_blender_tableau.cxx      vgui_blender_tableau.h    vgui_blender_tableau_sptr.h

  # tableau mixins and miscellaneous
  vgui_wrapper_tableau.cxx      vgui_wrapper_tableau.h    vgui_wrapper_tableau_sptr.h
  vgui_deck_tableau.cxx         vgui_deck_tableau.h       vgui_deck_tableau_sptr.h
  vgui_composite_tableau.cxx    vgui_composite_tableau.h  vgui_composite_tableau_sptr.h
  vgui_poly_tableau.cxx         vgui_poly_tableau.h       vgui_poly_tableau_sptr.h
  vgui_grid_tableau.cxx         vgui_grid_tableau.h       vgui_grid_tableau_sptr.h
  vgui_drag_mixin.cxx           vgui_drag_mixin.h
  vgui_active_tableau.cxx       vgui_active_tableau.h     vgui_active_tableau_sptr.h
  vgui_drag_tableau.cxx         vgui_drag_tableau.h       vgui_drag_tableau_sptr.h
  vgui_clear_tableau.cxx        vgui_clear_tableau.h      vgui_clear_tableau_sptr.h
  vgui_debug_tableau.cxx        vgui_debug_tableau.h      vgui_debug_tableau_sptr.h
  vgui_quit_tableau.cxx         vgui_quit_tableau.h       vgui_quit_tableau_sptr.h
  vgui_tview_launcher_tableau.cxx vgui_tview_launcher_tableau.h vgui_tview_launcher_tableau_sptr.h
  vgui_shell_tableau.cxx        vgui_shell_tableau.h      vgui_shell_tableau_sptr.h
  vgui_text_put.cxx             vgui_text_put.h
  vgui_text_tableau.cxx         vgui_text_tableau.h       vgui_text_tableau_sptr.h
  vgui_blackbox_tableau.cxx     vgui_blackbox_tableau.h   vgui_blackbox_tableau_sptr.h
  vgui_enhance_tableau.cxx      vgui_enhance_tableau.h    vgui_enhance_tableau_sptr.h
  vgui_tview_tableau.cxx        vgui_tview_tableau.h      vgui_tview_tableau_sptr.h
  vgui_function_tableau.cxx     vgui_function_tableau.h   vgui_function_tableau_sptr.h
  vgui_roi_tableau.cxx          vgui_roi_tableau.h        vgui_roi_tableau_sptr.h
  vgui_rubberband_tableau.cxx   vgui_rubberband_tableau.h vgui_rubberband_tableau_sptr.h
  vgui_loader_tableau.cxx       vgui_loader_tableau.h     vgui_loader_tableau_sptr.h
  vgui_color_text.cxx           vgui_color_text.h
  vgui_event_server.cxx         vgui_event_server.h
  vgui_error_dialog.cxx         vgui_error_dialog.h

  vgui_soview.cxx               vgui_soview.h
  vgui_soview2D.cxx             vgui_soview2D.h
  vgui_soview3D.cxx             vgui_soview3D.h
  vgui_style.cxx                vgui_style.h
  vgui_style_factory.cxx        vgui_style_factory.h
  vgui_displaybase_tableau.cxx  vgui_displaybase_tableau.h  vgui_displaybase_tableau_sptr.h
  vgui_displaylist2D_tableau.cxx  vgui_displaylist2D_tableau.h vgui_displaylist2D_tableau_sptr.h
  vgui_displaylist3D_tableau.cxx  vgui_displaylist3D_tableau.h vgui_displaylist3D_tableau_sptr.h
  vgui_easy2D_tableau.cxx       vgui_easy2D_tableau.h       vgui_easy2D_tableau_sptr.h
  vgui_easy3D_tableau.cxx       vgui_easy3D_tableau.h       vgui_easy3D_tableau_sptr.h
  vgui_listmanager2D_tableau.cxx  vgui_listmanager2D_tableau.h  vgui_listmanager2D_tableau_sptr.h
  vgui_viewer2D_tableau.cxx vgui_viewer2D_tableau.h         vgui_viewer2D_tableau_sptr.h
  vgui_viewer3D_tableau.cxx vgui_viewer3D_tableau.h         vgui_viewer3D_tableau_sptr.h
  internals/trackball.c              internals/trackball.h

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

  # extra header files
  vgui_gl.h
  vgui_glu.h
  vgui_glx.h

  # internals
  internals/vgui_invert_homg4x4.cxx  internals/vgui_invert_homg4x4.h
  internals/vgui_back_project.cxx    internals/vgui_back_project.h
  internals/vgui_un_project.cxx      internals/vgui_un_project.h
  internals/vgui_adaptor_tableau.cxx internals/vgui_adaptor_tableau.h
  internals/vgui_parent_child_link_data.cxx       internals/vgui_parent_child_link_data.h
  internals/vgui_adaptor_mixin.cxx   internals/vgui_adaptor_mixin.h
  internals/vgui_overlay_helper.cxx  internals/vgui_overlay_helper.h
  internals/vgui_multiply_4x4.cxx    internals/vgui_multiply_4x4.h
  internals/vgui_transpose_4x4.cxx   internals/vgui_transpose_4x4.h
  internals/vgui_draw_line.cxx       internals/vgui_draw_line.h
  internals/vgui_generic_vil_image_view.cxx       internals/vgui_generic_vil_image_view.h
  # Mesa-X11-mfc accelerated functions using Hermes
  internals/vgui_accelerate.cxx      internals/vgui_accelerate.h
  internals/vgui_accelerate_tag.cxx
  internals/vgui_gl_selection_macros.h



  # deprecated
  vgui_vil_image_tableau.txx         vgui_vil_image_tableau.h         vgui_vil_image_tableau_sptr.h
)

IF( GLUT_FOUND )
  SET(vgui_sources ${vgui_sources}
    vgui_glut.cxx                    vgui_glut.h
  )
ENDIF( GLUT_FOUND )

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
  #SET(vgui_sources ${vgui_sources}
  #  internals/vgui_accelerate_mfc.cxx internals/vgui_accelerate_mfc.h
  #)
ENDIF (MFC_FOUND)

IF (X11_FOUND)
  ADD_DEFINITIONS(-DHAS_X11)
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

    IF(CMAKE_COMPILER_IS_GNUCXX)
      SET_SOURCE_FILES_PROPERTIES(${CMAKE_CURRENT_BINARY_DIR}/moc_vgui_qt_adaptor.cxx GENERATED PROPERTIES COMPILE_FLAGS -O0)
    ENDIF(CMAKE_COMPILER_IS_GNUCXX)

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
      impl/mfc/vgui_mfc_utils.cxx       impl/mfc/vgui_mfc_utils.h
      impl/mfc/vgui_mfc_view.cxx        impl/mfc/vgui_mfc_view.h
      impl/mfc/vgui_mfc_window.cxx      impl/mfc/vgui_mfc_window.h
    )

    SET( vgui_sources ${vgui_sources} ${vgui_mfc_sources} )

  ENDIF(VGUI_USE_MFC)

ENDIF (MFC_FOUND)

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
# GTK2
##################################################

IF (GTK2_FOUND)

  OPTION(VGUI_USE_GTK2 "Should GTK2 support be compiled into vgui?" YES)

  IF(VGUI_USE_GTK2)

    IF(VGUI_USE_GTK)
       MESSAGE(SEND_ERROR "You cannot build against both GTK1 and"
                           "GTK2 at the same time")
    ENDIF(VGUI_USE_GTK)


    ADD_DEFINITIONS( "-DVGUI_USE_GTK2" )

    # This puts the include directories and whatever compiler flags
    # are necessary for GTK2
    ADD_DEFINITIONS( ${GTK2_DEFINITIONS} )

    SET(vgui_gtk2_sources
      impl/gtk2/vgui_gtk2.cxx             impl/gtk2/vgui_gtk2.h
      impl/gtk2/vgui_gtk2_adaptor.cxx     impl/gtk2/vgui_gtk2_adaptor.h
      impl/gtk2/vgui_gtk2_dialog_impl.cxx impl/gtk2/vgui_gtk2_dialog_impl.h
      impl/gtk2/vgui_gtk2_statusbar.cxx   impl/gtk2/vgui_gtk2_statusbar.h
      impl/gtk2/vgui_gtk2_tag.cxx
      impl/gtk2/vgui_gtk2_utils.cxx       impl/gtk2/vgui_gtk2_utils.h
      impl/gtk2/vgui_gtk2_window.cxx      impl/gtk2/vgui_gtk2_window.h
    )

    SET( vgui_sources ${vgui_sources} ${vgui_gtk2_sources} )

    AUX_SOURCE_DIRECTORY(impl/gtk2/Templates vgui_gtk2_sources)

  ENDIF(VGUI_USE_GTK2)

ENDIF (GTK2_FOUND)

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

# Dependencies on other vxl libraries
TARGET_LINK_LIBRARIES(vgui vnl_algo vnl vil1 vil vgl vpl vbl vul)

# Dependencies created by the toolkit implementations

IF (VGUI_USE_QT)
  # vul and vbl listed above. Don't repeat
  TARGET_LINK_LIBRARIES(vgui ${QT_LIBRARIES})
ENDIF (VGUI_USE_QT)
IF (VGUI_USE_MFC)
  # vul listed above. Don't repeat
  #TARGET_LINK_LIBRARIES(vgui vul)
ENDIF (VGUI_USE_MFC)
IF (VGUI_USE_GTK)
  # vul listed above. Don't repeat.
  TARGET_LINK_LIBRARIES(vgui ${GTK_LIBRARIES})
ENDIF (VGUI_USE_GTK)
IF (VGUI_USE_GTK2)
  # vul listed above. Don't repeat.
  TARGET_LINK_LIBRARIES(vgui ${GTK2_LIBRARIES})
ENDIF (VGUI_USE_GTK2)

# Basic vgui dependencies.
# These should appear after the above, because most of
# the toolkits depend on OpenGL.

IF (VGUI_USE_GLUT)
  TARGET_LINK_LIBRARIES(vgui ${GLUT_LIBRARIES})
ENDIF (VGUI_USE_GLUT)
IF (QV_FOUND)
  TARGET_LINK_LIBRARIES(vgui ${QV_LIBRARIES})
ENDIF (QV_FOUND)

TARGET_LINK_LIBRARIES(vgui ${OPENGL_LIBRARIES})

IF (X11_FOUND)
  TARGET_LINK_LIBRARIES(vgui ${X11_LIBRARIES})
ENDIF (X11_FOUND)

IF (BUILD_EXAMPLES)
  SUBDIRS(examples)
ENDIF (BUILD_EXAMPLES)

IF (BUILD_TESTING)
  SUBDIRS(tests)
ENDIF (BUILD_TESTING)

ENDIF (OPENGL_GLU_FOUND)
ENDIF (OPENGL_FOUND)

IF( NOT CAN_BUILD_VGUI )
  # Dashboard builds will try to build vgui if possible,
  # so don't warn if we can't.
  IF( NOT DART_ROOT )
    MESSAGE(SEND_ERROR "vgui could not be built.\n"
                       "Make sure the OpenGL libraries "
                       "(GL and GLU) are specified correctly,\n"
                       "or set BUILD_VGUI to OFF." )
  ENDIF( NOT DART_ROOT )
ENDIF( NOT CAN_BUILD_VGUI )

ENDIF( BUILD_VGUI )
