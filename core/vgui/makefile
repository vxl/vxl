USES += vxl oxl
USES += v3p # for Qv, only needed by vrml
USE_OPENGL:=1
TRY_GLUT := 1 # vgui_text needs glut. fsm.
TRY_X11 := 1
LIBRARY := vgui
IMMEDIATE_LIBS += -lvil -lvbl -lvnl-algo -lvnl -lvul

# basic classes
SOURCES += vgui.cxx
SOURCES += vgui_toolkit.cxx
SOURCES += vgui_tag.cxx
SOURCES += vgui_window.cxx
SOURCES += vgui_adaptor.cxx
SOURCES += vgui_tableau.cxx
SOURCES += vgui_slot.cxx
SOURCES += vgui_key.cxx
SOURCES += vgui_button.cxx
SOURCES += vgui_modifier.cxx
SOURCES += vgui_event.cxx
SOURCES += vgui_command.cxx
SOURCES += vgui_menu.cxx
SOURCES += vgui_popup_params.cxx
SOURCES += vgui_message.cxx
SOURCES += vgui_observer.cxx
SOURCES += vgui_observable.cxx
MANPAGE_SOURCES += vgui_satellite.txx

# utility
SOURCES += vgui_find.cxx
SOURCES += vgui_utils.cxx
SOURCES += vgui_macro.cxx
SOURCES += vgui_color.cxx
SOURCES += vgui_projection_inspector.cxx
SOURCES += vgui_matrix_state.cxx
SOURCES += vgui_text_graph.cxx
SOURCES += vgui_camera.cxx
SOURCES += vgui_event_condition.cxx

# images
SOURCES += internals/vgui_rasterpos.cxx
SOURCES += vgui_pixel.cxx
SOURCES += vgui_section_render.cxx
SOURCES += vgui_section_buffer.cxx
SOURCES += vgui_section_buffer_of.cxx
SOURCES += vgui_texture_hacks.cxx
SOURCES += vgui_cache_wizard.cxx
SOURCES += vgui_image_renderer.cxx
SOURCES += vgui_image_tableau.cxx
SOURCES += vgui_image_blender.cxx

# tableau mixins and miscellaneous
SOURCES += vgui_wrapper_tableau.cxx
SOURCES += vgui_deck_tableau.cxx
SOURCES += vgui_composite.cxx
SOURCES += vgui_polytab.cxx
SOURCES += vgui_grid_tableau.cxx
SOURCES += vgui_drag_mixin.cxx
SOURCES += vgui_active_visible.cxx
SOURCES += vgui_drag_tableau.cxx
SOURCES += vgui_clear_tableau.cxx
SOURCES += vgui_debug_tableau.cxx
SOURCES += vgui_quit_tableau.cxx
SOURCES += vgui_tview_launcher.cxx
SOURCES += vgui_shell_tableau.cxx
SOURCES += vgui_text_put.cxx
SOURCES += vgui_text_tableau.cxx
SOURCES += vgui_blackbox.cxx
SOURCES += vgui_enhance.cxx
SOURCES += vgui_tview.cxx
SOURCES += vgui_function_tableau.cxx
SOURCES += vgui_test.cxx
SOURCES += vgui_roi_tableau.cxx
SOURCES += vgui_rubberbander.cxx
SOURCES += vgui_load.cxx
SOURCES += vgui_color_text.cxx
SOURCES += vgui_event_server.cxx
SOURCES += vgui_error_dialog.cxx

# pcp
SOURCES += vgui_soview.cxx
SOURCES += vgui_soview2D.cxx
SOURCES += vgui_soview3D.cxx
SOURCES += vgui_style.cxx
SOURCES += vgui_style_factory.cxx
SOURCES += vgui_displaybase.cxx
SOURCES += vgui_displaylist2D.cxx
SOURCES += vgui_displaylist3D.cxx
SOURCES += vgui_easy2D.cxx
SOURCES += vgui_easy3D.cxx
SOURCES += vgui_listmanager2D.cxx
SOURCES += vgui_viewer2D.cxx
SOURCES += internals/trackball.c      # name
SOURCES += vgui_viewer3D.cxx

# dialog classes
SOURCES += vgui_dialog.cxx
SOURCES += internals/vgui_dialog_field.cxx
SOURCES += internals/vgui_simple_field.cxx
SOURCES += internals/vgui_string_field.cxx
SOURCES += internals/vgui_file_field.cxx
SOURCES += internals/vgui_dialog_impl.cxx

# statusbar classes
SOURCES += vgui_statusbuf.cxx
SOURCES += vgui_statusbar.cxx

# VRML I/O
SOURCES += vrml/vgui_vrml_texture_map.cxx
SOURCES += vrml/vgui_vrml_draw_visitor.cxx
SOURCES += vrml/vgui_vrml_tableau.cxx

#
SOURCES += vgui_glut.cxx
SOURCES += internals/vgui_invert_homg4x4.cxx
SOURCES += internals/vgui_back_project.cxx
SOURCES += internals/vgui_un_project.cxx
SOURCES += internals/vgui_adaptor_tableau.cxx
SOURCES += internals/vgui_slot_data.cxx
SOURCES += internals/vgui_adaptor_mixin.cxx
SOURCES += internals/vgui_overlay_helper.cxx
SOURCES += internals/vgui_multiply_4x4.cxx
SOURCES += internals/vgui_transpose_4x4.cxx
SOURCES += internals/vgui_draw_line.cxx

# experiment with new event model
SOURCES += vgui_slab.cxx
SOURCES += vgui_event_loop.cxx

# extra header files
MANPAGE_SOURCES += vgui_fwd.h
MANPAGE_SOURCES += vgui_gl.h
MANPAGE_SOURCES += vgui_glu.h
MANPAGE_SOURCES += vgui_glx.h
MANPAGE_SOURCES += vgui_linker_hack.h
MANPAGE_SOURCES += vgui_deck.h

MANPAGE_SOURCES += vgui_active_visible_sptr.h
MANPAGE_SOURCES += vgui_blackbox_sptr.h
MANPAGE_SOURCES += vgui_clear_tableau_sptr.h
MANPAGE_SOURCES += vgui_composite_sptr.h
MANPAGE_SOURCES += vgui_debug_tableau_sptr.h
MANPAGE_SOURCES += vgui_deck_tableau_sptr.h
MANPAGE_SOURCES += vgui_displaybase_sptr.h
MANPAGE_SOURCES += vgui_displaylist2D_sptr.h
MANPAGE_SOURCES += vgui_easy2D_sptr.h
MANPAGE_SOURCES += vgui_easy3D_sptr.h
MANPAGE_SOURCES += vgui_enhance_sptr.h
MANPAGE_SOURCES += vgui_grid_tableau_sptr.h
MANPAGE_SOURCES += vgui_image_tableau_sptr.h
MANPAGE_SOURCES += vgui_listmanager2D_sptr.h
MANPAGE_SOURCES += vgui_load_sptr.h
MANPAGE_SOURCES += vgui_polytab_sptr.h
MANPAGE_SOURCES += vgui_quit_tableau_sptr.h
MANPAGE_SOURCES += vgui_roi_tableau_sptr.h
MANPAGE_SOURCES += vgui_rubberbander_sptr.h
MANPAGE_SOURCES += vgui_shell_tableau_sptr.h
MANPAGE_SOURCES += vgui_tableau_sptr.h
MANPAGE_SOURCES += vgui_test_sptr.h
MANPAGE_SOURCES += vgui_text_tableau_sptr.h
MANPAGE_SOURCES += vgui_tview_launcher_sptr.h
MANPAGE_SOURCES += vgui_tview_sptr.h
MANPAGE_SOURCES += vgui_viewer2D_sptr.h
MANPAGE_SOURCES += vgui_viewer3D_sptr.h
MANPAGE_SOURCES += vgui_wrapper_tableau_sptr.h

#######################################
include ${IUEROOT}/config/top-params.mk
#######################################

# capes - Mesa-X11 accelerated functions using Hermes
SOURCES += internals/vgui_accelerate.cxx
SOURCES += internals/vgui_accelerate_tag.cxx
ifeq (1,$(HAS_X11))
SOURCES += internals/vgui_accelerate_x11.cxx
else
SOURCES += internals/vgui_accelerate_mfc.cxx
endif

# Compile all template instantiations in the following dirs (if these exist):
AUX_SOURCE_DIRECTORY += vrml/Templates internals/Templates

# -O2 does not turn on inlining, so make sure we use -O3 for egcs and 2.95.
# Also quell warning about truncating unsigned to fit bitfield.
ifeq ($(strip $(IU_COMPILER)),gcc-2.95)
optimize := -O3 -w
endif
ifeq ($(strip $(IU_COMPILER)),egcs)
optimize := -O3 -w
endif
ifeq ($(strip $(IU_COMPILER)),) #gcc
optimize := -O3 -w
endif
FLAGS_vgui_pixel.cxx += $(optimize)

#####################
# SUBDIRS
#####################
SUBDIRS += impl
#SUBDIRS += contrib
ifndef NOTEST
SUBDIRS += tests
SUBDIRS += examples
endif

######################################
include client.mk
include ${IUEROOT}/config/top-rules.mk
######################################

# on HP unix, this library has become too big, so it needs -fPIC
ifeq "HP-UX" "$(findstring HP-UX,$(OS))"
pic = $(PIC)
endif
