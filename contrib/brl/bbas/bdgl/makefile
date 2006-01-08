USES += contrib/brl contrib/brl/bbas contrib/gel core

LIBRARY := bdgl
VERSION := 1.0
DEFINES += -DBUILDING_BDGL_DLL

SOURCES += bdgl_curve_algs.cxx
SOURCES += bdgl_curve_region.cxx
SOURCES += bdgl_region_algs.cxx

#######################################
include ${VXLROOT}/config/top-params.mk
#######################################

ifndef NOTEST
 SUBDIRS += tests
endif

######################################
include ${VXLROOT}/config/top-rules.mk
######################################
