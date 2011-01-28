USES += contrib/brl contrib/brl/bseg core

LIBRARY := bvpl
VERSION := 1.0

MANPAGE_SOURCES += bvpl_voxel_subgrid.txx
MANPAGE_SOURCES += bvpl_subgrid_iterator.txx
MANPAGE_SOURCES += bvpl_neighb_operator.h
MANPAGE_SOURCES += bvpl_vector_operator.h
MANPAGE_SOURCES += bvpl_direction_to_color_map.h
MANPAGE_SOURCES += bvpl_susan_opinion_operator.h
SOURCES += bvpl_direction_to_color_map.cxx
MANPAGE_SOURCES += bvpl_combined_neighb_operator.h
MANPAGE_SOURCES += bvpl_discriminative_non_max_suppression.h

SUBDIRS += pro util bvpl_octree kernels functors

#######################################
include ${VXLROOT}/config/top-params.mk
#######################################

ifndef NOTEST
SUBDIRS += tests
endif

######################################
include ${VXLROOT}/config/top-rules.mk
######################################
