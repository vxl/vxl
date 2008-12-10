USES += contrib/brl contrib/brl/bbas contrib/brl/bseg contrib/gel/mrc core

LIBRARY := bvxm_rec
VERSION := 1.0

SOURCES += bvxm_bayesian_propagation.cxx
SOURCES += bvxm_bg_pair_density.cxx
SOURCES += bvxm_fg_bg_pair_density.cxx
SOURCES += bvxm_fg_pair_density.cxx
SOURCES += bvxm_hierarchy_edge.cxx
MANPAGE_SOURCES += bvxm_hierarchy_edge_sptr.h
SOURCES += bvxm_pair_density.cxx
SOURCES += bvxm_param_estimation.cxx
SOURCES += bvxm_part_base.cxx
MANPAGE_SOURCES += bvxm_part_base_sptr.h
SOURCES += bvxm_part_gaussian.cxx
MANPAGE_SOURCES += bvxm_part_gaussian_sptr.h
SOURCES += bvxm_part_hierarchy.cxx
SOURCES += bvxm_part_hierarchy_builder.cxx
MANPAGE_SOURCES += bvxm_part_hierarchy_sptr.h
SOURCES += bvxm_part_hierarchy_detector.cxx
SOURCES += bvxm_glitch.cxx

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
