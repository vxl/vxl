#
# ./oxl/vgui/CMakeLists.txt
#
#   geoff.cross@crd.ge.com
#

ADD_LIBRARY(vgui vgui_sources)

SOURCE_FILES(vgui_sources
vgui.cxx                   vgui_menu.cxx
vgui_active_visible.cxx    vgui_message.cxx
vgui_adaptor.cxx           vgui_modifier.cxx
vgui_blackbox.cxx          vgui_observable.cxx
vgui_button.cxx            vgui_observer.cxx
vgui_cache_wizard.cxx      vgui_pixel.cxx
vgui_camera.cxx            vgui_polytab.cxx
vgui_clear_tableau.cxx     vgui_popup_params.cxx
vgui_color.cxx             vgui_projection_inspector.cxx
vgui_color_text.cxx        vgui_quit_tableau.cxx
vgui_command.cxx           vgui_roi_tableau.cxx
vgui_composite.cxx         vgui_rubberbander.cxx
vgui_debug_tableau.cxx     vgui_section_buffer.cxx
vgui_deck.cxx              vgui_section_buffer_of.cxx
vgui_dialog.cxx            vgui_section_render.cxx
vgui_displaybase.cxx       vgui_shell_tableau.cxx
vgui_displaylist2D.cxx     vgui_slab.cxx
vgui_displaylist3D.cxx     vgui_slot.cxx
vgui_drag_mixin.cxx        vgui_soview.cxx
vgui_drag_tableau.cxx      vgui_soview2D.cxx
vgui_easy2D.cxx            vgui_soview3D.cxx
vgui_easy3D.cxx            vgui_statusbar.cxx
vgui_enhance.cxx           vgui_statusbuf.cxx
vgui_error_dialog.cxx      vgui_style.cxx
vgui_event.cxx             vgui_style_factory.cxx
vgui_event_condition.cxx   vgui_tableau.cxx
vgui_event_loop.cxx        vgui_tag.cxx
vgui_event_server.cxx      vgui_test.cxx
vgui_find.cxx              vgui_text_graph.cxx
vgui_function_tableau.cxx  vgui_text_put.cxx
vgui_glut.cxx              vgui_text_tableau.cxx
vgui_grid_tableau.cxx      vgui_texture_hacks.cxx
vgui_image_blender.cxx     vgui_toolkit.cxx
vgui_image_renderer.cxx    vgui_tview.cxx
vgui_image_tableau.cxx     vgui_tview_launcher.cxx
vgui_key.cxx               vgui_utils.cxx
vgui_listmanager2D.cxx     vgui_viewer2D.cxx
vgui_load.cxx              vgui_viewer3D.cxx
vgui_macro.cxx             vgui_window.cxx
vgui_matrix_state.cxx      vgui_wrapper_tableau.cxx
)

AUX_SOURCE_DIRECTORY(Templates vgui_sources)

SUBDIRS(impl internals) 


