# This is contrib/dbvxm/CMakeLists.txt

INCLUDE_DIRECTORIES( ${VXLCORE_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${GEL_INCLUDE_DIR})
INCLUDE_DIRECTORIES( ${GEL_INCLUDE_DIR}/mrc )
INCLUDE_DIRECTORIES( ${GEL_INCLUDE_DIR}/mrc/vpgl )
INCLUDE_DIRECTORIES( ${MUL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bbas )
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bbas/bsta )
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bseg/bvxm )

SET(bvxm_rec_sources
    bvxm_bayesian_propagation.h  bvxm_bayesian_propagation.cxx
    bvxm_param_estimation.h  bvxm_param_estimation.cxx
    bvxm_fg_bg_pair_density.h  bvxm_fg_bg_pair_density.cxx
    bvxm_fg_pair_density.h  bvxm_fg_pair_density.cxx
    bvxm_pair_density.h          bvxm_pair_density.cxx
    bvxm_bg_pair_density.h  bvxm_bg_pair_density.cxx
    bvxm_part_base.h     bvxm_part_base.cxx
    bvxm_part_base_sptr.h
    bvxm_part_gaussian.h    bvxm_part_gaussian.cxx
    bvxm_part_gaussian_sptr.h
    bvxm_part_hierarchy.h    bvxm_part_hierarchy.cxx
    bvxm_part_hierarchy_sptr.h
    bvxm_hierarchy_edge.h    bvxm_hierarchy_edge.cxx
    bvxm_hierarchy_edge_sptr.h
    bvxm_part_hierarchy_builder.h    bvxm_part_hierarchy_builder.cxx
    bvxm_part_hierarchy_detector.h    bvxm_part_hierarchy_detector.cxx
    bvxm_glitch.h    bvxm_glitch.cxx
)

AUX_SOURCE_DIRECTORY(Templates bvxm_rec_sources)

ADD_LIBRARY(bvxm_rec ${bvxm_rec_sources})

TARGET_LINK_LIBRARIES(bvxm_rec bvxm bsta_algo bgrl2 bsta bvgl bxml vil brip vgl_algo)

SUBDIRS( pro )

IF( BUILD_TESTING )
  SUBDIRS(tests)
ENDIF( BUILD_TESTING )



