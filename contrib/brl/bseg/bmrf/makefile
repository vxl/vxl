USES += contrib/brl contrib/brl/bseg contrib/gel core

LIBRARY := bmrf
VERSION := 1.0

SOURCES += bmrf_epi_point.cxx
MANPAGE_SOURCES += bmrf_epi_point_sptr.h
SOURCES += bmrf_epi_seg.cxx
MANPAGE_SOURCES += bmrf_epi_seg_sptr.h
SOURCES += bmrf_node.cxx
MANPAGE_SOURCES += bmrf_node_sptr.h
SOURCES += bmrf_network.cxx
MANPAGE_SOURCES += bmrf_network_sptr.h
SOURCES += bmrf_network_builder.cxx
SOURCES += bmrf_network_builder_params.cxx
SOURCES += bmrf_epi_transform.cxx
MANPAGE_SOURCES += bmrf_epi_transform_sptr.h
SOURCES += bmrf_epipole.cxx

#######################################
include ${IUEROOT}/config/top-params.mk
#######################################

ifndef NOTEST
 SUBDIRS += tests
endif

######################################
include ${IUEROOT}/config/top-rules.mk
######################################
