USES += contrib/brl contrib/brl/bbas contrib/brl/bseg core

LIBRARY := bvpl_octree

SOURCES += temp.cxx
MANPAGE_SOURCES += bvpl_octree_vector_operator.h
MANPAGE_SOURCES += bvpl_octree_vector_operator_impl.h
MANPAGE_SOURCES += bvpl_octree_kernel_operator.h
MANPAGE_SOURCES += bvpl_scene_kernel_operator.h
MANPAGE_SOURCES += bvpl_scene_vector_operator.h

#######################################
include ${VXLROOT}/config/top-params.mk
#######################################

ifndef NOTEST
 SUBDIRS += tests
endif

######################################
include ${VXLROOT}/config/top-rules.mk
######################################
