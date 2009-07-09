USES += contrib/brl contrib/brl/bbas core

LIBRARY := bsvg
VERSION := 1.0

SOURCES += bsvg_element.cxx
SOURCES += bsvg_document.cxx
SOURCES += bsvg_plot.cxx

SUBDIRS += pro

#######################################
include ${VXLROOT}/config/top-params.mk
#######################################

ifndef NOTEST
SUBDIRS += tests
endif

######################################
include ${VXLROOT}/config/top-rules.mk
######################################
