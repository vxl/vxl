# ./oxl/vgui/CMakeLists.txt

INCLUDE(${allvxl_SOURCE_DIR}/config.cmake/Modules/FindOpenGL.cmake)
INCLUDE(${allvxl_SOURCE_DIR}/config.cmake/Modules/FindGLUT.cmake)
INCLUDE(${allvxl_SOURCE_DIR}/config.cmake/Modules/FindX11.cmake)
# Qv needed by vrml only
INCLUDE(${allvxl_SOURCE_DIR}/config.cmake/Modules/FindQv.cmake)

# Build here only if OpenGL is available:
IF(HAS_OPENGL)

ADD_LIBRARY(vgui vgui_sources)

SOURCE_FILES(vgui_sources
  vgui.cxx
  vgui_toolkit.cxx
  vgui_tag.cxx
  vgui_window.cxx
  vgui_adaptor.cxx
  vgui_tableau.cxx
  vgui_slot.cxx
  vgui_key.cxx
  vgui_button.cxx
  vgui_modifier.cxx
  vgui_event.cxx
  vgui_command.cxx
  vgui_menu.cxx
  vgui_popup_params.cxx
  vgui_message.cxx
  vgui_observer.cxx
  vgui_observable.cxx
  vgui_find.cxx
  vgui_utils.cxx
  vgui_macro.cxx
  vgui_color.cxx
  vgui_projection_inspector.cxx
  vgui_matrix_state.cxx
  vgui_text_graph.cxx
  vgui_camera.cxx
  vgui_event_condition.cxx
  vgui_pixel.cxx
  vgui_section_render.cxx
  vgui_section_buffer.cxx
  vgui_section_buffer_of.cxx
  vgui_texture_hacks.cxx
  vgui_cache_wizard.cxx
  vgui_image_renderer.cxx
  vgui_image_tableau.cxx
  vgui_image_blender.cxx
  vgui_wrapper_tableau.cxx
  vgui_deck_tableau.cxx
  vgui_composite.cxx
  vgui_polytab.cxx
  vgui_grid_tableau.cxx
  vgui_drag_mixin.cxx
  vgui_active_visible.cxx
  vgui_drag_tableau.cxx
  vgui_clear_tableau.cxx
  vgui_debug_tableau.cxx
  vgui_quit_tableau.cxx
  vgui_tview_launcher.cxx
  vgui_shell_tableau.cxx
  vgui_text_put.cxx
  vgui_text_tableau.cxx
  vgui_blackbox.cxx
  vgui_enhance.cxx
  vgui_tview.cxx
  vgui_function_tableau.cxx
  vgui_test.cxx
  vgui_roi_tableau.cxx
  vgui_rubberbander.cxx
  vgui_load.cxx
  vgui_color_text.cxx
  vgui_event_server.cxx
  vgui_error_dialog.cxx
  vgui_soview.cxx
  vgui_soview2D.cxx
  vgui_soview3D.cxx
  vgui_style.cxx
  vgui_style_factory.cxx
  vgui_displaybase.cxx
  vgui_displaylist2D.cxx
  vgui_displaylist3D.cxx
  vgui_easy2D.cxx
  vgui_easy3D.cxx
  vgui_listmanager2D.cxx
  vgui_viewer2D.cxx
  vgui_viewer3D.cxx
  vgui_dialog.cxx
  vgui_statusbuf.cxx
  vgui_statusbar.cxx
  vgui_glut.cxx
  vgui_slab.cxx
  vgui_event_loop.cxx
  internals/vgui_rasterpos.cxx
  internals/trackball.c
  internals/vgui_dialog_field.cxx
  internals/vgui_simple_field.cxx
  internals/vgui_string_field.cxx
  internals/vgui_file_field.cxx
  internals/vgui_dialog_impl.cxx
  internals/vgui_invert_homg4x4.cxx
  internals/vgui_back_project.cxx
  internals/vgui_un_project.cxx
  internals/vgui_adaptor_tableau.cxx
  internals/vgui_slot_data.cxx
  internals/vgui_adaptor_mixin.cxx
  internals/vgui_overlay_helper.cxx
  internals/vgui_multiply_4x4.cxx
  internals/vgui_transpose_4x4.cxx
  internals/vgui_draw_line.cxx
  internals/vgui_accelerate.cxx
  internals/vgui_accelerate_tag.cxx
  vrml/vgui_vrml_draw_visitor.cxx
  vrml/vgui_vrml_tableau.cxx
  vrml/vgui_vrml_texture_map.cxx
)

IF(HAS_MFC)
  SOURCE_FILES(vgui_sources
    internals/vgui_accelerate_mfc.cxx
  )
ENDIF(HAS_MFC)

IF(HAS_X11)
  SOURCE_FILES(vgui_sources
    internals/vgui_accelerate_x11.cxx
  )
ENDIF(HAS_X11)

AUX_SOURCE_DIRECTORY(Templates vgui_sources)
AUX_SOURCE_DIRECTORY(vrml/Templates vgui_sources)

IF(HAS_MFC)
  # add stuff to use MFC in this executable
  ADD_DEFINITIONS(-D_AFXDLL)
  SET(CMAKE_MFC_FLAG 6)
ENDIF(HAS_MFC)

IF(WIN32)
  IF(NOT CYGWIN)
    INCLUDE(${allvxl_SOURCE_DIR}/oxl/vgui/CMakeListsHeaders.txt)
  ENDIF(NOT CYGWIN)
ENDIF(WIN32)

SUBDIRS(impl)

IF( BUILD_EXAMPLES )
  SUBDIRS( examples )
ENDIF( BUILD_EXAMPLES )

IF( BUILD_TESTING )
  SUBDIRS(tests)
ENDIF( BUILD_TESTING )

ENDIF(HAS_OPENGL)
