USES += contrib/brl contrib/brl/bbas contrib/brl/bseg contrib/gel core

LIBRARY := segv
VERSION := 1.0

MANPAGE_SOURCES += dll.h
MANPAGE_SOURCES += vil_dll.h
SOURCES += segv_segmentation_manager.cxx
SOURCES += segv_vtol2D_tableau.cxx
MANPAGE_SOURCES += segv_vtol2D_tableau_sptr.h
SOURCES += segv_vtol_soview2D.cxx
SOURCES += segv_menus.cxx
SOURCES += segv_vil_menus.cxx
SOURCES += segv_vil_segmentation_manager.cxx

#######################################
include ${IUEROOT}/config/top-params.mk
#######################################

ifndef NOTEST
 SUBDIRS += tests
endif

######################################
include ${IUEROOT}/config/top-rules.mk
######################################
