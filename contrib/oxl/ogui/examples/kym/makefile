USES += contrib/oxl core
include ${IUEROOT}/core/vgui/config.mk

MINI_PROG_SOURCES += display-image.cxx
MINI_PROG_SOURCES += draw-line.cxx
MINI_PROG_SOURCES += 2d-example.cxx
MINI_PROG_SOURCES += 3d-example.cxx
MINI_PROG_SOURCES += menubar-example.cxx
MINI_PROG_SOURCES += popup-example.cxx
MINI_PROG_SOURCES += mouse-position.cxx
MINI_PROG_SOURCES += mouse-draw-line.cxx
MINI_PROG_SOURCES += dialog-example.cxx
MINI_PROG_SOURCES += rubberband.cxx
MINI_PROG_SOURCES += printing_example.cxx

#######################################
include ${IUEROOT}/config/top-params.mk
#######################################

ifeq ($(USE_GTKGLAREA),1)
MINI_PROG_SOURCES += gtk-display-image.cxx
MINI_PROG_SOURCES += gtk-movie-player.cxx
ifneq (,$(strip $(wildcard $(IUEROOT)/core/vidl/config.mk)))
include ${IUEROOT}/core/vidl/config.mk
MINI_PROG_SOURCES += vidl-demo.cxx
endif
endif

######################################
include ${IUEROOT}/config/top-rules.mk
######################################
