# This is contrib/brl/bseg/bvpl/CMakeLists.txt

INCLUDE_DIRECTORIES( ${VXLCORE_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bseg )

SET(bvpl_sources
    bvpl_voxel_subgrid.h               bvpl_voxel_subgrid.txx
    bvpl_subgrid_iterator.h            bvpl_subgrid_iterator.txx
    bvpl_kernel_iterator.h
    bvpl_edge2d_functor.h              bvpl_edge2d_functor.txx
    bvpl_opinion_functor.h             bvpl_opinion_functor.cxx
    bvpl_kernel_factory.h              bvpl_kernel_factory.cxx
    bvpl_edge2d_kernel_factory.h       bvpl_edge2d_kernel_factory.cxx
    bvpl_edge3d_kernel_factory.h       bvpl_edge3d_kernel_factory.cxx
    bvpl_edge_algebraic_mean_functor.h bvpl_edge_algebraic_mean_functor.txx
    bvpl_edge_geometric_mean_functor.h bvpl_edge_geometric_mean_functor.txx
    bvpl_neighb_operator.h
    bvpl_vector_operator.h
    bvpl_direction_to_color_map.h
   )

AUX_SOURCE_DIRECTORY(Templates bvpl_sources)

ADD_LIBRARY(bvpl ${bvpl_sources})

TARGET_LINK_LIBRARIES(bvpl bvxm_grid vgl bdgl bxml bsvg)

SUBDIRS( pro )
SUBDIRS( io )

IF( BUILD_TESTING )
  SUBDIRS(tests)
ENDIF( BUILD_TESTING )
