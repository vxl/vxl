USES += contrib/oxl core
include ${IUEROOT}/core/vgui/config.mk
USE_FLTK := 1

SOURCES += Fl_twowin_ui.cxx
SOURCES += Fl_movie_ui.cxx
SOURCES += Fl_blender_ui.cxx

MINI_PROG_SOURCES += test_movie.cxx
OBJECTS_test_movie += $(RELOBJDIR)/Fl_movie_ui.o 

MINI_PROG_SOURCES += test_twowin.cxx
OBJECTS_test_twowin += $(RELOBJDIR)/Fl_twowin_ui.o

MINI_PROG_SOURCES += test_blender.cxx
OBJECTS_test_blender += $(RELOBJDIR)/Fl_blender_ui.o

################################
include ${IUEROOT}/config/top.mk
################################
