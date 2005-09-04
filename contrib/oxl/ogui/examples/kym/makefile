USES += contrib/oxl core
include ${VXLROOT}/core/vgui/config.mk

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
include ${VXLROOT}/config/top-params.mk
#######################################

ifeq ($(USE_GTKGLAREA),1)
MINI_PROG_SOURCES += gtk-display-image.cxx
MINI_PROG_SOURCES += gtk-movie-player.cxx
ifneq (,$(strip $(wildcard $(VXLROOT)/core/vidl_vil1/config.mk)))
include ${VXLROOT}/core/vidl_vil1/config.mk
MINI_PROG_SOURCES += vidl-demo.cxx
endif
endif

######################################
include ${VXLROOT}/config/top-rules.mk
######################################
