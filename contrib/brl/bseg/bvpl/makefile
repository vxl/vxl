USES += contrib/brl contrib/brl/bseg core

LIBRARY := bvpl
VERSION := 1.0

MANPAGE_SOURCES += bvpl_voxel_subgrid.txx
MANPAGE_SOURCES += bvpl_subgrid_iterator.txx
MANPAGE_SOURCES += bvpl_kernel_iterator.h
MANPAGE_SOURCES += bvpl_edge2d_functor.txx
MANPAGE_SOURCES += bvpl_neighb_operator.h
SOURCES += bvpl_kernel_factory.cxx
SOURCES += bvpl_edge2d_kernel_factory.cxx
SOURCES += bvpl_opinion_functor.cxx
SOURCES += bvpl_edge3d_kernel_factory.cxx
MANPAGE_SOURCES += bvpl_vector_operator.h
MANPAGE_SOURCES += bvpl_edge_algebraic_mean_functor.txx
MANPAGE_SOURCES += bvpl_edge_geometric_mean_functor.txx
MANPAGE_SOURCES += bvpl_direction_to_color_map.h
MANPAGE_SOURCES += bvpl_local_max_functor.h
SOURCES += bvpl_gauss3d_xx_kernel_factory.cxx

SUBDIRS += pro io

#######################################
include ${VXLROOT}/config/top-params.mk
#######################################

ifndef NOTEST
SUBDIRS += tests
endif

######################################
include ${VXLROOT}/config/top-rules.mk
######################################
