USES += contrib/brl contrib/brl/bseg core

LIBRARY := bvpl
VERSION := 1.0

MANPAGE_SOURCES += bvpl_voxel_subgrid.txx
MANPAGE_SOURCES += bvpl_subgrid_iterator.txx
MANPAGE_SOURCES += bvpl_kernel_iterator.h
SOURCES += bvpl_kernel_base.cxx
SOURCES += bvpl_edge2d_kernel.cxx
MANPAGE_SOURCES += bvpl_edge2d_functor.txx
MANPAGE_SOURCES += bvpl_neighb_operator.txx

#######################################
include ${VXLROOT}/config/top-params.mk
#######################################

ifndef NOTEST
SUBDIRS += tests
endif

######################################
include ${VXLROOT}/config/top-rules.mk
######################################
