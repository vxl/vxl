USES += contrib/brl contrib/brl/bbas core

LIBRARY := volm
VERSION := 1.0

SOURCES += volm_spherical_container.cxx
SOURCES += volm_query.cxx
SOURCES += volm_io.cxx
SOURCES += volm_tile.cxx
MANPAGE_SOURCES += volm_query_sptr.h
MANPAGE_SOURCES += volm_spherical_container_sptr.h

#######################################
include ${VXLROOT}/config/top-params.mk
#######################################

ifndef NOTEST
SUBDIRS += tests exe
endif

######################################
include ${VXLROOT}/config/top-rules.mk
######################################
