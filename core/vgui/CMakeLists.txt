# oxl/vgui/CMakeLists.txt

INCLUDE(${MODULE_PATH}/FindOpenGL.cmake)
INCLUDE(${MODULE_PATH}/FindGLUT.cmake)
INCLUDE(${MODULE_PATH}/FindX11.cmake)
# Qv needed by vrml only
INCLUDE(${MODULE_PATH}/FindQv.cmake)

# Build here only if OpenGL is available:
IF (HAS_OPENGL)

INCLUDE_DIRECTORIES(${OPENGL_INCLUDE_PATH})

SOURCE_FILES(vgui_sources
 vgui_fwd.h

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
 vgui_find.cxx                 vgui_find.h
 vgui_utils.cxx                vgui_utils.h
 vgui_macro.cxx                vgui_macro.h
 vgui_color.cxx                vgui_color.h
 vgui_projection_inspector.cxx vgui_projection_inspector.h
 vgui_matrix_state.cxx         vgui_matrix_state.h
 vgui_text_graph.cxx           vgui_text_graph.h
 vgui_camera.cxx               vgui_camera.h
 vgui_event_condition.cxx      vgui_event_condition.h
 vgui_pixel.cxx                vgui_pixel.h
 vgui_section_render.cxx       vgui_section_render.h
 vgui_section_buffer.cxx       vgui_section_buffer.h
 vgui_section_buffer_of.cxx    vgui_section_buffer_of.h
 vgui_texture_hacks.cxx        vgui_texture_hacks.h
 vgui_cache_wizard.cxx         vgui_cache_wizard.h
 vgui_image_renderer.cxx       vgui_image_renderer.h
 vgui_image_tableau.cxx        vgui_image_tableau.h    vgui_image_tableau_sptr.h
 vgui_image_blender.cxx        vgui_image_blender.h
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
 vgui_viewer3D.cxx             vgui_viewer3D.h         vgui_viewer3D_sptr.h
 vgui_dialog.cxx               vgui_dialog.h
 vgui_statusbuf.cxx            vgui_statusbuf.h
 vgui_statusbar.cxx            vgui_statusbar.h
 vgui_slab.cxx                 vgui_slab.h
 vgui_event_loop.cxx           vgui_event_loop.h

 vgui_satellite.txx            vgui_satellite.h
 vgui_deck.h
 vgui_gl.h
 vgui_glu.h
 vgui_glx.h
 vgui_linker_hack.h

 internals/vgui_rasterpos.cxx       internals/vgui_rasterpos.h
 internals/trackball.c              internals/trackball.h
 internals/vgui_dialog_field.cxx    internals/vgui_dialog_field.h
 internals/vgui_simple_field.cxx    internals/vgui_simple_field.h
 internals/vgui_string_field.cxx    internals/vgui_string_field.h
 internals/vgui_file_field.cxx      internals/vgui_file_field.h
 internals/vgui_dialog_impl.cxx     internals/vgui_dialog_impl.h
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
 internals/vgui_accelerate.cxx      internals/vgui_accelerate.h
 internals/vgui_accelerate_tag.cxx
)

IF (HAS_QV)
  INCLUDE_DIRECTORIES(${QV_INCLUDE_PATH})
  SOURCE_FILES(vgui_sources
   vrml/vgui_vrml_draw_visitor.cxx  vrml/vgui_vrml_draw_visitor.h
   vrml/vgui_vrml_tableau.cxx       vrml/vgui_vrml_tableau.h
   vrml/vgui_vrml_texture_map.cxx   vrml/vgui_vrml_texture_map.h
  )
ENDIF (HAS_QV)

IF (HAS_MFC)
  SOURCE_FILES(vgui_sources
   internals/vgui_accelerate_mfc.cxx internals/vgui_accelerate_mfc.h
  )
ENDIF (HAS_MFC)

IF (HAS_X11)
  INCLUDE_DIRECTORIES(${X11_INCLUDE_PATH})
  SOURCE_FILES(vgui_sources
   internals/vgui_accelerate_x11.cxx internals/vgui_accelerate_x11.h
  )
ENDIF (HAS_X11)

IF (HAS_GLUT)
  INCLUDE_DIRECTORIES(${GLUT_INCLUDE_PATH})
  SOURCE_FILES(vgui_sources vgui_sources
   vgui_glut.cxx vgui_glut.h
  )
ENDIF (HAS_GLUT)

AUX_SOURCE_DIRECTORY(Templates vgui_sources)
AUX_SOURCE_DIRECTORY(vrml/Templates vgui_sources)

IF (HAS_MFC)
  # add stuff to use MFC in this executable
  ADD_DEFINITIONS( "-D_AFXDLL" )
  SET(CMAKE_MFC_FLAG 6)
ENDIF (HAS_MFC)

ADD_LIBRARY(vgui_base vgui_sources)

TARGET_LINK_LIBRARIES(vgui_base vnl_algo vnl vil vgl vpl vbl vul)
IF (HAS_GLUT)
  TARGET_LINK_LIBRARIES(vgui_base ${GLUT_LIBRARIES})
ENDIF (HAS_GLUT)
IF (HAS_QV)
  TARGET_LINK_LIBRARIES(vgui_base ${QV_LIBRARIES})
ENDIF (HAS_QV)

TARGET_LINK_LIBRARIES(vgui_base ${OPENGL_LIBRARIES})

IF (HAS_X11)
  TARGET_LINK_LIBRARIES(vgui_base ${X11_LIBRARIES})
ENDIF (HAS_X11)


SUBDIRS(impl)

# The vgui_IMPL_* variables will be set by the different
# implementations. The variable will be a library name or empty.
# Unlike the general FindXXX modules, we don't have (or need) a
# HAS_IMPL_xxx variable. If the variable is set, then we have found
# the toolkit AND the user has allowed us to compile it in.

IF (vgui_IMPL_GLUT)
  ADD_DEFINITIONS( "-DVGUI_USE_GLUT" )
ENDIF (vgui_IMPL_GLUT)
IF (vgui_IMPL_QT)
  ADD_DEFINITIONS( "-DVGUI_USE_QT" )
ENDIF (vgui_IMPL_QT)
IF (vgui_IMPL_MFC)
  ADD_DEFINITIONS( "-DVGUI_USE_MFC" )
ENDIF (vgui_IMPL_MFC)
IF (vgui_IMPL_X)
  ADD_DEFINITIONS( "-DVGUI_USE_X" )
ENDIF (vgui_IMPL_X)
IF (vgui_IMPL_GLX)
  ADD_DEFINITIONS( "-DVGUI_USE_GLX" )
ENDIF (vgui_IMPL_GLX)
IF (vgui_IMPL_GTK)
  ADD_DEFINITIONS( "-DVGUI_USE_GTK" )
ENDIF (vgui_IMPL_GTK)
IF (vgui_IMPL_SDL)
  ADD_DEFINITIONS( "-DVGUI_USE_SDL" )
ENDIF (vgui_IMPL_SDL)
IF (vgui_IMPL_XM)
  ADD_DEFINITIONS( "-DVGUI_USE_XM" )
ENDIF (vgui_IMPL_XM)
IF (vgui_IMPL_FL)
  ADD_DEFINITIONS( "-DVGUI_USE_FL" )
ENDIF (vgui_IMPL_FL)


ADD_LIBRARY( vgui
  vgui_link_all.cxx
)
TARGET_LINK_LIBRARIES( vgui vgui_base )

IF (vgui_IMPL_GLUT)
  TARGET_LINK_LIBRARIES(vgui ${vgui_IMPL_GLUT})
ENDIF (vgui_IMPL_GLUT)
IF (vgui_IMPL_QT)
  TARGET_LINK_LIBRARIES(vgui ${vgui_IMPL_QT})
ENDIF (vgui_IMPL_QT)
IF (vgui_IMPL_MFC)
  TARGET_LINK_LIBRARIES(vgui ${vgui_IMPL_MFC})
ENDIF (vgui_IMPL_MFC)
IF (vgui_IMPL_X)
  TARGET_LINK_LIBRARIES(vgui ${vgui_IMPL_X})
ENDIF (vgui_IMPL_X)
IF (vgui_IMPL_GLX)
  TARGET_LINK_LIBRARIES(vgui ${vgui_IMPL_GLX})
ENDIF (vgui_IMPL_GLX)
IF (vgui_IMPL_GTK)
  TARGET_LINK_LIBRARIES(vgui ${vgui_IMPL_GTK})
ENDIF (vgui_IMPL_GTK)
IF (vgui_IMPL_SDL)
  TARGET_LINK_LIBRARIES(vgui ${vgui_IMPL_SDL})
ENDIF (vgui_IMPL_SDL)
IF (vgui_IMPL_XM)
  TARGET_LINK_LIBRARIES(vgui ${vgui_IMPL_XM})
ENDIF (vgui_IMPL_XM)
IF (vgui_IMPL_FL)
  TARGET_LINK_LIBRARIES(vgui ${vgui_IMPL_FL})
ENDIF (vgui_IMPL_FL)


IF (BUILD_EXAMPLES)
  SUBDIRS(examples)
ENDIF (BUILD_EXAMPLES)

IF (BUILD_TESTING)
  SUBDIRS(tests)
ENDIF (BUILD_TESTING)

ENDIF (HAS_OPENGL)
