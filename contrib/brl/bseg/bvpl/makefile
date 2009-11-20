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
SOURCES += bvpl_local_max_functor.cxx
SOURCES += bvpl_gauss3d_xx_kernel_factory.cxx
SOURCES += bvpl_gauss_convolution_functor.cxx
MANPAGE_SOURCES += bvpl_susan_opinion_operator.h
SOURCES += bvpl_create_directions.cxx
SOURCES += bvpl_direction_to_color_map.cxx
SOURCES += bvpl_kernel.cxx
SOURCES += bvpl_corner2d_kernel_factory.cxx
SOURCES += bvpl_find_surface_functor.cxx
MANPAGE_SOURCES += bvpl_combined_neighb_operator.h
SOURCES += bvpl_weighted_cube_kernel_factory.cxx
SOURCES += bvpl_neighborhood_kernel_factory.cxx
SOURCES += bvpl_positive_gauss_conv_functor.cxx
SOURCES += bvpl_negative_gauss_conv_functor.cxx
MANPAGE_SOURCES += bvpl_discriminative_non_max_suppression.h

SUBDIRS += pro io util bvpl_octree

#######################################
include ${VXLROOT}/config/top-params.mk
#######################################

ifndef NOTEST
SUBDIRS += tests
endif

######################################
include ${VXLROOT}/config/top-rules.mk
######################################
