USES += vxl oxl
USE_OPENGL:=1
TRY_GLUT := 1 # vgui_text needs glut. fsm.
TRY_X11 := 1
TRY_HERMES := 1
LIBRARY := vgui
IMMEDIATE_LIBS += -lvil -lvbl -lvnl-algo -lvnl -lvul

#######################################
include ${IUEROOT}/config/top-params.mk
#######################################

# basic classes
SOURCES += vgui.cxx
SOURCES += vgui_toolkit.cxx
SOURCES += vgui_tag.cxx
SOURCES += vgui_window.cxx
SOURCES += vgui_adaptor.cxx
SOURCES += vgui_tableau.cxx
MANPAGE_SOURCES += vgui_tableau_sptr.h
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
FLAGS_vgui_pixel.cxx += $(optimize)
SOURCES += vgui_section_render.cxx
SOURCES += vgui_section_buffer.cxx
SOURCES += vgui_section_buffer_of.cxx
SOURCES += vgui_texture_hacks.cxx
SOURCES += vgui_cache_wizard.cxx
SOURCES += vgui_image_renderer.cxx
SOURCES += vgui_image_tableau.cxx
MANPAGE_SOURCES += vgui_image_tableau_sptr.h
SOURCES += vgui_image_blender.cxx

# tableau mixins and miscellaneous
SOURCES += vgui_wrapper_tableau.cxx
MANPAGE_SOURCES += vgui_wrapper_tableau_sptr.h
SOURCES += vgui_deck_tableau.cxx
MANPAGE_SOURCES += vgui_deck_tableau_sptr.h
SOURCES += vgui_composite.cxx
MANPAGE_SOURCES += vgui_composite_sptr.h
SOURCES += vgui_polytab.cxx
MANPAGE_SOURCES += vgui_polytab_sptr.h
SOURCES += vgui_grid_tableau.cxx
MANPAGE_SOURCES += vgui_grid_tableau_sptr.h
SOURCES += vgui_drag_mixin.cxx
SOURCES += vgui_active_visible.cxx
MANPAGE_SOURCES += vgui_active_visible_sptr.h
SOURCES += vgui_drag_tableau.cxx
SOURCES += vgui_clear_tableau.cxx
MANPAGE_SOURCES += vgui_clear_tableau_sptr.h
SOURCES += vgui_debug_tableau.cxx
MANPAGE_SOURCES += vgui_debug_tableau_sptr.h
SOURCES += vgui_quit_tableau.cxx
MANPAGE_SOURCES += vgui_quit_tableau_sptr.h
SOURCES += vgui_tview_launcher.cxx
MANPAGE_SOURCES += vgui_tview_launcher_sptr.h
SOURCES += vgui_shell_tableau.cxx
MANPAGE_SOURCES += vgui_shell_tableau_sptr.h
SOURCES += vgui_text_put.cxx
SOURCES += vgui_text_tableau.cxx
MANPAGE_SOURCES += vgui_text_tableau_sptr.h
SOURCES += vgui_blackbox.cxx
MANPAGE_SOURCES += vgui_blackbox_sptr.h
SOURCES += vgui_enhance.cxx
MANPAGE_SOURCES += vgui_enhance_sptr.h
SOURCES += vgui_tview.cxx
MANPAGE_SOURCES += vgui_tview_sptr.h
SOURCES += vgui_function_tableau.cxx
SOURCES += vgui_roi_tableau.cxx
MANPAGE_SOURCES += vgui_roi_tableau_sptr.h
SOURCES += vgui_rubberbander.cxx
MANPAGE_SOURCES += vgui_rubberbander_sptr.h
SOURCES += vgui_load.cxx
MANPAGE_SOURCES += vgui_load_sptr.h
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
MANPAGE_SOURCES += vgui_displaybase_sptr.h
SOURCES += vgui_displaylist2D.cxx
MANPAGE_SOURCES += vgui_displaylist2D_sptr.h
SOURCES += vgui_displaylist3D.cxx
SOURCES += vgui_easy2D.cxx
MANPAGE_SOURCES += vgui_easy2D_sptr.h
SOURCES += vgui_easy3D.cxx
MANPAGE_SOURCES += vgui_easy3D_sptr.h
SOURCES += vgui_listmanager2D.cxx
MANPAGE_SOURCES += vgui_listmanager2D_sptr.h
SOURCES += vgui_viewer2D.cxx
MANPAGE_SOURCES += vgui_viewer2D_sptr.h
SOURCES += internals/trackball.c      # name
SOURCES += vgui_viewer3D.cxx
MANPAGE_SOURCES += vgui_viewer3D_sptr.h

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

#
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
MANPAGE_SOURCES += vgui_deck.h
MANPAGE_SOURCES += vgui_gl.h
MANPAGE_SOURCES += vgui_glu.h
MANPAGE_SOURCES += vgui_glx.h
MANPAGE_SOURCES += vgui_linker_hack.h

# Mesa-X11 accelerated functions using Hermes
SOURCES += internals/vgui_accelerate.cxx
SOURCES += internals/vgui_accelerate_tag.cxx
ifeq (1,$(HAS_X11))
SOURCES += internals/vgui_accelerate_x11.cxx
else
SOURCES += internals/vgui_accelerate_mfc.cxx
endif

# VRML I/O -- uses Qv
USES += v3p
SOURCES += vrml/vgui_vrml_texture_map.cxx
SOURCES += vrml/vgui_vrml_draw_visitor.cxx
SOURCES += vrml/vgui_vrml_tableau.cxx
AUX_SOURCE_DIRECTORY += vrml/Templates

ifeq ($(HAS_X11),1)
SOURCES += impl/X/vgui_X_utils.cxx	
SOURCES += impl/glX/vgui_glX.cxx	
SOURCES += impl/glX/vgui_glX_adaptor.cxx	
SOURCES += impl/glX/vgui_glX_tag.cxx	
SOURCES += impl/glX/vgui_glX_window.cxx	
AUX_SOURCE_DIRECTORY += impl/glX/Templates
endif

ifeq ($(HAS_FLTK),1)
SOURCES += impl/Fl/vgui_Fl.cxx	
SOURCES += impl/Fl/vgui_Fl_adaptor.cxx	
SOURCES += impl/Fl/vgui_Fl_dialog_impl.cxx	
SOURCES += impl/Fl/vgui_Fl_statusbar.cxx	
SOURCES += impl/Fl/vgui_Fl_tag.cxx	
SOURCES += impl/Fl/vgui_Fl_utils.cxx	
SOURCES += impl/Fl/vgui_Fl_window.cxx	
endif

ifeq ($(HAS_GLUT),1)
SOURCES += vgui_glut.cxx
SOURCES += impl/glut/vgui_glut_adaptor.cxx	
SOURCES += impl/glut/vgui_glut_impl.cxx	
SOURCES += impl/glut/vgui_glut_popup_impl.cxx	
SOURCES += impl/glut/vgui_glut_slab.cxx	
SOURCES += impl/glut/vgui_glut_tag.cxx	
SOURCES += impl/glut/vgui_glut_window.cxx	
MANPAGE_SOURCES += impl/glut/menu_hack.h
ifeq ($(HAS_X11),1)
SOURCES += impl/glut/menu_hack_X11.cxx
else
SOURCES += impl/glut/menu_hack_none.cxx
endif
AUX_SOURCE_DIRECTORY += impl/glut/Templates
endif

ifeq ($(HAS_MOTIFGL),1)
SOURCES += impl/Xm/vgui_Xm.cxx	
SOURCES += impl/Xm/vgui_Xm_adaptor.cxx	
SOURCES += impl/Xm/vgui_Xm_dialog_impl.cxx	
SOURCES += impl/Xm/vgui_Xm_statusbar.cxx	
SOURCES += impl/Xm/vgui_Xm_tag.cxx	
SOURCES += impl/Xm/vgui_Xm_utils.cxx	
SOURCES += impl/Xm/vgui_Xm_window.cxx	
AUX_SOURCE_DIRECTORY += impl/Xm/Templates
endif

ifeq ($(HAS_GTKGLAREA),1)
INCDIRS += $(GTKGLAREA_INC_DIR)
DEFINES += $(GTKCONFIGCFLAGS)
SOURCES += impl/gtk/vgui_gtk.cxx	
SOURCES += impl/gtk/vgui_gtk_adaptor.cxx	
SOURCES += impl/gtk/vgui_gtk_dialog_impl.cxx	
SOURCES += impl/gtk/vgui_gtk_statusbar.cxx	
SOURCES += impl/gtk/vgui_gtk_tag.cxx	
SOURCES += impl/gtk/vgui_gtk_utils.cxx	
SOURCES += impl/gtk/vgui_gtk_window.cxx	
AUX_SOURCE_DIRECTORY += impl/gtk/Templates
endif

ifeq ($(HAS_SDL),1)
SOURCES += impl/SDL/vgui_SDL.cxx	
SOURCES += impl/SDL/vgui_SDL_adaptor.cxx	
SOURCES += impl/SDL/vgui_SDL_factory.cxx	
SOURCES += impl/SDL/vgui_SDL_tag.cxx	
SOURCES += impl/SDL/vgui_SDL_window.cxx	
endif

ifeq ($(HAS_QGL), 1)
SOURCES += impl/qt/vgui_qt.cxx	
SOURCES += impl/qt/vgui_qt_adaptor.cxx	
SOURCES += impl/qt/vgui_qt_dialog_impl.cxx	
SOURCES += impl/qt/vgui_qt_menu.cxx	
SOURCES += impl/qt/vgui_qt_statusbar.cxx	
SOURCES += impl/qt/vgui_qt_tag.cxx	
SOURCES += impl/qt/vgui_qt_window.cxx	

CPPEXT = cxx
HPPEXT = h
ALLHEADERS := $(wildcard $(SOURCES:.$(CPPEXT)=.$(HPPEXT)))
MOCHEADERS := $(shell fgrep -l Q_OBJECT $(ALLHEADERS))
MOCSOURCES := $(MOCHEADERS:.$(HPPEXT)=_mocced.$(CPPEXT))

SOURCES += ${MOCSOURCES}
DEFINES += -DQT_NO_STL

ifeq ($(OS),OSF1V4)
FLAGS_impl/qt/vgui_qt_adaptor_mocced.cxx := -O0
# because of an "internal compile error" of gcc2.95 whenever optimisation is on, even -O1
endif

endif

ifeq ($(HAS_MFC), 1)
SOURCES += impl/mfc/vgui_mfc.cxx	
SOURCES += impl/mfc/vgui_mfc_adaptor.cxx	
SOURCES += impl/mfc/vgui_mfc_app.cxx	
SOURCES += impl/mfc/vgui_mfc_app_init.cxx	
SOURCES += impl/mfc/vgui_mfc_dialog_impl.cxx	
SOURCES += impl/mfc/vgui_mfc_doc.cxx	
SOURCES += impl/mfc/vgui_mfc_mainfrm.cxx	
SOURCES += impl/mfc/vgui_mfc_statusbar.cxx	
SOURCES += impl/mfc/vgui_mfc_tag.cxx	
SOURCES += impl/mfc/vgui_mfc_util.cxx	
SOURCES += impl/mfc/vgui_mfc_utils.cxx	
SOURCES += impl/mfc/vgui_mfc_view.cxx	
SOURCES += impl/mfc/vgui_mfc_window.cxx	
endif

# This file uses the -DVGUI_USE_* preprocessor definitions set by
# the implementations to register each toolkit that was compiled in.
SOURCES += vgui_register_all.cxx


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

#####################
# SUBDIRS
#####################
ifndef NOTEST
SUBDIRS += tests
SUBDIRS += examples
endif

######################################
include ${IUEROOT}/config/top-rules.mk
######################################

# on HP unix, this library has become too big, so it needs -fPIC
ifeq "HP-UX" "$(findstring HP-UX,$(OS))"
pic = $(PIC)
endif

ifeq ($(HAS_QGL), 1)
%_mocced.$(CPPEXT): %.$(HPPEXT)
	@echo "--- Moccing $<"
	$(MOC) $< -o $@

mocclean:
	find . -name \*_mocced.\* -print | xargs $(RM) ; true

cleaner: mocclean clean

endif
