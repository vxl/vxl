USES += vxl oxl
TRY_X11 := 1

USE_OPENGL := 1
USE_GLUT   := 1

LIBRARY := vgui-glut

IMMEDIATE_LIBS += -lvgui

SOURCES += vgui_glut_impl.cxx
SOURCES += vgui_glut_tag.cxx
SOURCES += vgui_glut_window.cxx
SOURCES += vgui_glut_adaptor.cxx
SOURCES += vgui_glut_popup_impl.cxx

SOURCES += vgui_glut_slab.cxx

#######################################
include ${IUEROOT}/config/top-params.mk
#######################################

MANPAGE_SOURCES += menu_hack.h
ifeq ($(HAS_X11),1)
SOURCES += menu_hack_X11.cxx
else
SOURCES += menu_hack_none.cxx
endif

######################################
include ${IUEROOT}/config/top-rules.mk
######################################
