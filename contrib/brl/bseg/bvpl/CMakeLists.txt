# This is contrib/brl/bseg/bvpl/CMakeLists.txt

INCLUDE_DIRECTORIES( ${VXLCORE_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bseg )
INCLUDE_DIRECTORIES( ${GEL_INCLUDE_DIR}/mrc )

SET(bvpl_sources
    bvpl_voxel_subgrid.h               bvpl_voxel_subgrid.txx
    bvpl_subgrid_iterator.h            bvpl_subgrid_iterator.txx
    bvpl_kernel.h                      bvpl_kernel.cxx
    bvpl_kernel_iterator.h
    bvpl_edge2d_functor.h              bvpl_edge2d_functor.txx
    bvpl_opinion_functor.h             bvpl_opinion_functor.cxx
    bvpl_kernel_factory.h              bvpl_kernel_factory.cxx
    bvpl_edge2d_kernel_factory.h       bvpl_edge2d_kernel_factory.cxx
    bvpl_edge3d_kernel_factory.h       bvpl_edge3d_kernel_factory.cxx
    bvpl_gauss3d_xx_kernel_factory.h   bvpl_gauss3d_xx_kernel_factory.cxx
    bvpl_corner2d_kernel_factory.h     bvpl_corner2d_kernel_factory.cxx
    bvpl_weighted_cube_kernel_factory.h bvpl_weighted_cube_kernel_factory.cxx
    bvpl_neighborhood_kernel_factory.h bvpl_neighborhood_kernel_factory.cxx
    bvpl_edge_algebraic_mean_functor.h bvpl_edge_algebraic_mean_functor.txx
    bvpl_edge_geometric_mean_functor.h bvpl_edge_geometric_mean_functor.txx
    bvpl_gauss_convolution_functor.h   bvpl_gauss_convolution_functor.cxx
    bvpl_positive_gauss_conv_functor.h bvpl_positive_gauss_conv_functor.cxx
    bvpl_negative_gauss_conv_functor.h bvpl_negative_gauss_conv_functor.cxx
    bvpl_find_surface_functor.h        bvpl_find_surface_functor.cxx
    bvpl_local_max_functor.h           bvpl_local_max_functor.cxx
    bvpl_neighb_operator.h
    bvpl_combined_neighb_operator.h
    bvpl_vector_operator.h
    bvpl_discriminative_non_max_suppression.h
    bvpl_direction_to_color_map.h	   bvpl_direction_to_color_map.cxx
    bvpl_susan_opinion_operator.h
    bvpl_create_directions.h		   bvpl_create_directions.cxx
   )

AUX_SOURCE_DIRECTORY(Templates bvpl_sources)

ADD_LIBRARY(bvpl ${bvpl_sources})

TARGET_LINK_LIBRARIES(bvpl bvxm_grid vgl bdgl bxml bsvg bsta vpl)

SUBDIRS( pro )
SUBDIRS( io )
SUBDIRS( util )

IF( BUILD_TESTING )
  SUBDIRS(tests)
ENDIF( BUILD_TESTING )
