USES += contrib/brl contrib/brl/bseg contrib/gel core

LIBRARY := bmrf
VERSION := 1.0

SOURCES += bmrf_epi_point.cxx
SOURCES += bmrf_epi_seg.cxx
SOURCES += bmrf_node.cxx
SOURCES += bmrf_network.cxx
SOURCES += bmrf_network_builder.cxx
SOURCES += bmrf_network_builder_params.cxx
MANPAGE_SOURCES += bmrf_epi_point_sptr.h
MANPAGE_SOURCES += bmrf_epi_seg_sptr.h
MANPAGE_SOURCES += bmrf_node_sptr.h
MANPAGE_SOURCES += bmrf_network_sptr.h

#######################################
include ${IUEROOT}/config/top-params.mk
#######################################

ifndef NOTEST
 SUBDIRS += tests
endif

######################################
include ${IUEROOT}/config/top-rules.mk
######################################
