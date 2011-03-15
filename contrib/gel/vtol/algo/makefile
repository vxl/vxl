USES += contrib/gel core

IMMEDIATE_LIBS += -lvtol

LIBRARY := vtol-algo
VERSION := 1.0

SOURCES += vtol_extract_topology.cxx
MANPAGE_SOURCES += vtol_extract_topology.txx
#SOURCES += vtol_extract_topology_internals.cxx

#######################################
include ${VXLROOT}/config/top-params.mk
#######################################

ifndef NOTEST
SUBDIRS += tests
endif

######################################
include ${VXLROOT}/config/top-rules.mk
######################################
