USES += contrib/brl contrib/brl/bbas contrib/brl/bseg contrib/gel core

LIBRARY := bmrf
VERSION := 1.0

SOURCES += bmrf_epipole.cxx
SOURCES += bmrf_epi_point.cxx
MANPAGE_SOURCES += bmrf_epi_point_sptr.h
SOURCES += bmrf_epi_seg.cxx
MANPAGE_SOURCES += bmrf_epi_seg_sptr.h
SOURCES += bmrf_gamma_func.cxx
MANPAGE_SOURCES += bmrf_gamma_func_sptr.h
SOURCES += bmrf_epi_transform.cxx
SOURCES += bmrf_node.cxx
MANPAGE_SOURCES += bmrf_node_sptr.h
SOURCES += bmrf_network.cxx
MANPAGE_SOURCES += bmrf_network_sptr.h
SOURCES += bmrf_network_builder.cxx
SOURCES += bmrf_network_builder_params.cxx
SOURCES += bmrf_curvel_3d.cxx
MANPAGE_SOURCES += bmrf_curvel_3d_sptr.h
SOURCES += bmrf_curve_3d_builder.cxx

#######################################
include ${IUEROOT}/config/top-params.mk
#######################################

ifndef NOTEST
 SUBDIRS += tests
endif

######################################
include ${IUEROOT}/config/top-rules.mk
######################################
