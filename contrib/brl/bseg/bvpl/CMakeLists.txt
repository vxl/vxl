# This is contrib/brl/bseg/bvpl/CMakeLists.txt
PROJECT(bvpl)

INCLUDE_DIRECTORIES( ${VXLCORE_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bseg )
INCLUDE_DIRECTORIES( ${GEL_INCLUDE_DIR}/mrc )

SET(bvpl_sources
    bvpl_voxel_subgrid.h               bvpl_voxel_subgrid.txx
    bvpl_subgrid_iterator.h            bvpl_subgrid_iterator.txx
    bvpl_neighb_operator.h
    bvpl_combined_neighb_operator.h
    bvpl_vector_operator.h
    bvpl_discriminative_non_max_suppression.h
    bvpl_direction_to_color_map.h      bvpl_direction_to_color_map.cxx
    bvpl_susan_opinion_operator.h
   )

AUX_SOURCE_DIRECTORY(Templates bvpl_sources)

ADD_LIBRARY(bvpl ${bvpl_sources})

TARGET_LINK_LIBRARIES(bvpl bvpl_kernels bvpl_functors bvxm_grid bdgl bsta vnl vnl_io vil_algo vgl vpl vul vbl_io vsl vbl)

SUBDIRS( pro )
SUBDIRS( util )
SUBDIRS( bvpl_octree )
SUBDIRS( kernels )
SUBDIRS( functors )

IF( BUILD_TESTING )
  SUBDIRS(tests)
ENDIF( BUILD_TESTING )
