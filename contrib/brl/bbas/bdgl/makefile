USES += contrib/brl contrib/brl/bbas contrib/gel core

LIBRARY := bdgl
VERSION := 1.0

SOURCES += bdgl_curve_algs.cxx
SOURCES += bdgl_curve_region.cxx
SOURCES += bdgl_region_algs.cxx

#######################################
include ${IUEROOT}/config/top-params.mk
#######################################

ifndef NOTEST
 SUBDIRS += tests
endif

######################################
include ${IUEROOT}/config/top-rules.mk
######################################
