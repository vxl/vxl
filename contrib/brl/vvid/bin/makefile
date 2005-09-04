USES += contrib/brl contrib/brl/bbas contrib/brl/bseg core
include ../config.mk

LIBRARY := vvid_bin
SOURCES += vidfpl_menus.cxx
ifdef HAS_CMU1394CAMERA
SOURCES += videx_menus.cxx
SOURCES += vidster_menus.cxx
endif
SOURCES += vidfpl_vil.cxx
SOURCES += vidfpl_vil_menus.cxx

MINI_PROG_SOURCES += vidfpl.cxx
MINI_PROG_SOURCES += command_line_grid_finder.cxx
ifdef HAS_CMU1394CAMERA
MINI_PROG_SOURCES += videx.cxx
MINI_PROG_SOURCES += vidster.cxx
endif

IULIBS += -lvvid_bin

################################
include ${VXLROOT}/config/top.mk
################################
