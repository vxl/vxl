USES += vxl oxl
USE_OPENGL := 1
USE_QGL := 1

LIBRARY := vgui-qt

SOURCES += vgui_qt.cxx
SOURCES += vgui_qt_tag.cxx
SOURCES += vgui_qt_window.cxx
SOURCES += vgui_qt_adaptor.cxx
SOURCES += vgui_qt_statusbar.cxx
SOURCES += vgui_qt_dialog_impl.cxx
SOURCES += vgui_qt_menu.cxx

CPPEXT = cxx
HPPEXT = h
ALLHEADERS := $(wildcard $(SOURCES:.$(CPPEXT)=.$(HPPEXT)))
MOCHEADERS := $(shell fgrep -l Q_OBJECT $(ALLHEADERS))
MOCSOURCES := $(MOCHEADERS:.$(HPPEXT)=_mocced.$(CPPEXT))

SOURCES += ${MOCSOURCES}

################################
include ${IUEROOT}/config/top.mk
################################

ifeq ($(OS),OSF1V4)
FLAGS_vgui_qt_adaptor_mocced.cxx := -O0
# because of an "internal compile error" of gcc2.95 whenever optimisation is on, even -O1
endif

%_mocced.$(CPPEXT): %.$(HPPEXT)
	@echo "--- Moccing $<"
	$(MOC) $< -o $@

mocclean:
	find . -name \*_mocced.\* -print | xargs $(RM) ; true

cleaner: mocclean clean

