#
# ./oxl/vgui/CMakeLists.txt
#

# add stuff to use MFC in this executable
IF(WIN32)
 ADD_DEFINITIONS(-D_AFXDLL)
 SET(CMAKE_MFC_FLAG 6)
ENDIF(WIN32)

ADD_LIBRARY(vgui vgui_sources)

SOURCE_FILES(vgui_sources
 vgui
 vgui_toolkit
 vgui_tag
 vgui_window
 vgui_adaptor
 vgui_tableau
 vgui_slot
 vgui_key
 vgui_button
 vgui_modifier
 vgui_event
 vgui_command
 vgui_menu
 vgui_popup_params
 vgui_message
 vgui_observer
 vgui_observable
 vgui_find
 vgui_utils
 vgui_macro
 vgui_color
 vgui_projection_inspector
 vgui_matrix_state
 vgui_text_graph
 vgui_camera
 vgui_event_condition
 vgui_pixel
 vgui_section_render
 vgui_section_buffer
 vgui_section_buffer_of
 vgui_texture_hacks
 vgui_cache_wizard
 vgui_image_renderer
 vgui_image_tableau
 vgui_image_blender
 vgui_wrapper_tableau
 vgui_deck_tableau
 vgui_composite
 vgui_polytab
 vgui_grid_tableau
 vgui_drag_mixin
 vgui_active_visible
 vgui_drag_tableau
 vgui_clear_tableau
 vgui_debug_tableau
 vgui_quit_tableau
 vgui_tview_launcher
 vgui_shell_tableau
 vgui_text_put
 vgui_text_tableau
 vgui_blackbox
 vgui_enhance
 vgui_tview
 vgui_function_tableau
 vgui_test
 vgui_roi_tableau
 vgui_rubberbander
 vgui_load
 vgui_color_text
 vgui_event_server
 vgui_error_dialog
 vgui_soview
 vgui_soview2D
 vgui_soview3D
 vgui_style
 vgui_style_factory
 vgui_displaybase
 vgui_displaylist2D
 vgui_displaylist3D
 vgui_easy2D
 vgui_easy3D
 vgui_listmanager2D
 vgui_viewer2D
 vgui_viewer3D
 vgui_dialog
 vgui_statusbuf
 vgui_statusbar
 vgui_glut
 vgui_slab
 vgui_event_loop
 internals/vgui_rasterpos
 internals/trackball
 internals/vgui_dialog_field
 internals/vgui_simple_field
 internals/vgui_string_field
 internals/vgui_file_field
 internals/vgui_dialog_impl
 internals/vgui_invert_homg4x4
 internals/vgui_back_project
 internals/vgui_un_project
 internals/vgui_adaptor_tableau
 internals/vgui_slot_data
 internals/vgui_adaptor_mixin
 internals/vgui_overlay_helper
 internals/vgui_multiply_4x4
 internals/vgui_transpose_4x4
 internals/vgui_draw_line
 internals/vgui_accelerate
 internals/vgui_accelerate_tag
 vrml/vgui_vrml_draw_visitor
 vrml/vgui_vrml_tableau
 vrml/vgui_vrml_texture_map
)

INCLUDE( ${CMAKE_SOURCE_DIR}/config.cmake/Modules/FindGLUT.cmake )
INCLUDE( ${CMAKE_SOURCE_DIR}/config.cmake/Modules/FindOpenGL.cmake )
INCLUDE( ${CMAKE_SOURCE_DIR}/config.cmake/Modules/FindX11.cmake )
# Qv needed by vrml
INCLUDE( ${CMAKE_SOURCE_DIR}/config.cmake/Modules/FindQv.cmake )

IF(WIN32)
  SOURCE_FILES(vgui_sources
    internals/vgui_accelerate_mfc
  )
ENDIF(WIN32)

IF(HAS_X11)
  SOURCE_FILES(vgui_sources
    internals/vgui_accelerate_x11
  )
ENDIF(HAS_X11)

AUX_SOURCE_DIRECTORY(Templates vgui_sources)
AUX_SOURCE_DIRECTORY(vrml/Templates vgui_sources)

SUBDIRS(impl)
SUBDIRS(examples)

IF(BUILD_TESTING)
  SUBDIRS(tests) 
ENDIF(BUILD_TESTING)
