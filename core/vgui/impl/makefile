#######################################
include ${IUEROOT}/config/top-params.mk
#######################################

# vgui_toolkit implementations.

ifeq ($(HAS_X11),1)
SUBDIRS += X
SUBDIRS += glX
endif

ifeq ($(HAS_FLTK),1)
SUBDIRS += Fl
endif

ifeq ($(HAS_GLUT),1)
SUBDIRS += glut
endif

ifeq ($(HAS_MOTIFGL),1)
SUBDIRS += Xm
endif

ifeq ($(HAS_GTKGLAREA),1)
SUBDIRS += gtk
endif

ifeq ($(HAS_SDL),1)
SUBDIRS += SDL
endif

ifeq ($(HAS_QGL), 1)
SUBDIRS += qt
endif

ifeq ($(HAS_MFC), 1)
SUBDIRS += mfc
endif

######################################
include ${IUEROOT}/config/top-rules.mk
######################################
