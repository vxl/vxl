USES += contrib/brl contrib/gel contrib/brl/bbas core

LIBRARY := bdgl
VERSION := 1.0

SOURCES += bdgl_curve_algs.cxx
SOURCES += bdgl_curve_description.cxx
SOURCES += bdgl_curve_matcher.cxx
SOURCES += bdgl_curve_region.cxx
SOURCES += bdgl_curve_tracker.cxx
SOURCES += bdgl_curve_tracker_primitive.cxx

#######################################
include ${IUEROOT}/config/top-params.mk
#######################################

ifndef NOTEST
 SUBDIRS += tests
endif

######################################
include ${IUEROOT}/config/top-rules.mk
######################################
