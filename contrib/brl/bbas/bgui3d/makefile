USES += contrib/brl contrib/gel contrib/brl/bbas contrib/gel/mrc core

LIBRARY := bgui3d
VERSION := 1.0

SOURCES += bgui3d.cxx
SOURCES += bgui3d_algo.cxx
SOURCES += bgui3d_examiner_slider_tableau.cxx
MANPAGE_SOURCES += bgui3d_examiner_slider_tableau_sptr.h
SOURCES += bgui3d_examiner_tableau.cxx
MANPAGE_SOURCES += bgui3d_examiner_tableau_sptr.h
SOURCES += bgui3d_file_io.cxx
SOURCES += bgui3d_fullviewer_tableau.cxx
MANPAGE_SOURCES += bgui3d_fullviewer_tableau_sptr.h
SOURCES += bgui3d_project2d_tableau.cxx
MANPAGE_SOURCES += bgui3d_project2d_tableau_sptr.h
SOURCES += bgui3d_tableau.cxx
MANPAGE_SOURCES += bgui3d_tableau_sptr.h
SOURCES += bgui3d_translate_event.cxx
SOURCES += bgui3d_viewer_tableau.cxx
MANPAGE_SOURCES += bgui3d_viewer_tableau_sptr.h

#######################################
include ${VXLROOT}/config/top-params.mk
#######################################

ifndef NOTEST
SUBDIRS += examples
endif

######################################
include ${VXLROOT}/config/top-rules.mk
######################################
