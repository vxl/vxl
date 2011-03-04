# Sources  for appearance and mixed grid algorithms
INCLUDE_DIRECTORIES( ${VXLCORE_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bbas)
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bseg/bvxm )

SET(bvxm_algo_sources
    bvxm_merge_mog.h     bvxm_merge_mog.cxx
    bvxm_mog_norm.h      bvxm_mog_norm.txx
    bvxm_boxm_convert.h
   )

AUX_SOURCE_DIRECTORY(Templates bvxm_algo_sources)

ADD_LIBRARY( bvxm_algo ${bvxm_algo_sources})

TARGET_LINK_LIBRARIES( bvxm_algo bsta bvxm_grid_io boxm_util vnl vgl)

SUBDIRS(pro)

IF( BUILD_TESTING )
  SUBDIRS(tests)
ENDIF( BUILD_TESTING )
