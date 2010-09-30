USES += contrib/brl contrib/brl/bbas contrib/brl/bseg core

LIBRARY := bvpl_octree

SOURCES += bvpl_scene_neighborhood_operator.cxx
MANPAGE_SOURCES += bvpl_octree_vector_operator.h
MANPAGE_SOURCES += bvpl_octree_vector_operator_impl.h
MANPAGE_SOURCES += bvpl_octree_kernel_operator.h
MANPAGE_SOURCES += bvpl_scene_kernel_operator.h
MANPAGE_SOURCES += bvpl_scene_vector_operator.h
MANPAGE_SOURCES += bvpl_octree_sample.txx
MANPAGE_SOURCES += bvpl_octree_vrml_util.h
MANPAGE_SOURCES += bvpl_cell_data_traits.h
MANPAGE_SOURCES += bvpl_octree_neighbors.h
MANPAGE_SOURCES += bvpl_scene_statistics.h

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
