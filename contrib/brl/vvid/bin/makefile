USES += contrib/brl contrib/gel contrib/oxl core
include ../config.mk

LIBRARY := vvid_bin
SOURCES += vidfpl_menus.cxx
ifdef HAS_CMU1394CAMERA
SOURCES += videx_menus.cxx
endif

MINI_PROG_SOURCES += vidfpl.cxx
ifdef HAS_CMU1394CAMERA
MINI_PROG_SOURCES += videx.cxx
endif

IULIBS += -lvvid_bin

################################
include ${IUEROOT}/config/top.mk
################################
