#
# ./oxl/vgui/CMakeLists.txt
#
#   geoff.cross@crd.ge.com
#

ADD_LIBRARY(vgui vgui_sources)

SOURCE_FILES(vgui_sources
vgui                   vgui_menu
vgui_active_visible    vgui_message
vgui_adaptor           vgui_modifier
vgui_blackbox          vgui_observable
vgui_button            vgui_observer
vgui_cache_wizard      vgui_pixel
vgui_camera            vgui_polytab
vgui_clear_tableau     vgui_popup_params
vgui_color             vgui_projection_inspector
vgui_color_text        vgui_quit_tableau
vgui_command           vgui_roi_tableau
vgui_composite         vgui_rubberbander
vgui_debug_tableau     vgui_section_buffer
vgui_deck              vgui_section_buffer_of
vgui_dialog            vgui_section_render
vgui_displaybase       vgui_shell_tableau
vgui_displaylist2D     vgui_slab
vgui_displaylist3D     vgui_slot
vgui_drag_mixin        vgui_soview
vgui_drag_tableau      vgui_soview2D
vgui_easy2D            vgui_soview3D
vgui_easy3D            vgui_statusbar
vgui_enhance           vgui_statusbuf
vgui_error_dialog      vgui_style
vgui_event             vgui_style_factory
vgui_event_condition   vgui_tableau
vgui_event_loop        vgui_tag
vgui_event_server      vgui_test
vgui_find              vgui_text_graph
vgui_function_tableau  vgui_text_put
vgui_glut              vgui_text_tableau
vgui_grid_tableau      vgui_texture_hacks
vgui_image_blender     vgui_toolkit
vgui_image_renderer    vgui_tview
vgui_image_tableau     vgui_tview_launcher
vgui_key               vgui_utils
vgui_listmanager2D     vgui_viewer2D
vgui_load              vgui_viewer3D
vgui_macro             vgui_window
vgui_matrix_state      vgui_wrapper_tableau
)

INCLUDE_DIRECTORIES(${OPENGL_INCLUDE_PATH})

AUX_SOURCE_DIRECTORY(Templates vgui_sources)

SUBDIRS(impl internals) 


