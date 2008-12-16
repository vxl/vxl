USES += contrib/brl contrib/brl/bbas contrib/brl/bseg core

LIBRARY := brec

SOURCES += brec_hierarchy_edge.cxx
SOURCES += brec_part_hierarchy_builder.cxx
SOURCES += brec_part_hierarchy.cxx
SOURCES += brec_glitch.cxx
SOURCES += brec_part_hierarchy_detector.cxx
SOURCES += brec_bg_pair_density.cxx
SOURCES += brec_fg_pair_density.cxx
SOURCES += brec_fg_bg_pair_density.cxx
SOURCES += brec_part_base.cxx
SOURCES += brec_bayesian_propagation.cxx
SOURCES += brec_part_gaussian.cxx
SOURCES += brec_pair_density.cxx
SOURCES += brec_param_estimation.cxx
MANPAGE_SOURCES += brec_part_gaussian_sptr.h
MANPAGE_SOURCES += brec_part_hierarchy_sptr.h
MANPAGE_SOURCES += brec_part_base_sptr.h
MANPAGE_SOURCES += brec_hierarchy_edge_sptr.h

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
