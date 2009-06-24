# This is contrib/brl/bseg/bvpl/CMakeLists.txt

INCLUDE_DIRECTORIES( ${VXLCORE_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bseg )

SET(bvpl_sources
    bvpl_voxel_subgrid.txx           bvpl_voxel_subgrid.h
    bvpl_subgrid_iterator.txx        bvpl_subgrid_iterator.h
    bvpl_kernel_iterator.h
    bvpl_edge2d_functor.txx          bvpl_edge2d_functor.h
    bvpl_opinion_functor.cxx         bvpl_opinion_functor.h
    bvpl_kernel_factory.h            bvpl_kernel_factory.cxx
    bvpl_edge2d_kernel_factory.h     bvpl_edge2d_kernel_factory.cxx
    bvpl_neighb_operator.h  
    bvpl_vector_operator.h         
)

AUX_SOURCE_DIRECTORY(Templates bvpl_sources)

ADD_LIBRARY(bvpl ${bvpl_sources})

TARGET_LINK_LIBRARIES(bvpl bvxm_grid vgl)

SUBDIRS( pro )
SUBDIRS( io )

IF( BUILD_TESTING )
  SUBDIRS(tests)
ENDIF( BUILD_TESTING )
