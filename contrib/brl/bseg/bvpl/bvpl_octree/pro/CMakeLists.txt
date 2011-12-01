# This is contrib/brl/bseg/bvpl/pro/CMakeLists.txt
# bvxm's Process Library

# bprb depends on expat and bvxm_pro depends on bprb primarily
INCLUDE( ${BRL_MODULE_PATH}/FindEXPAT.cmake )
IF(EXPAT_FOUND)
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bpro )
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bpro/bprb )

SET(bvpl_octree_pro_sources
    bvpl_octree_processes.h
    bvpl_octree_register.h    bvpl_octree_register.cxx
   )

AUX_SOURCE_DIRECTORY(Templates bvpl_octree_pro_sources)
AUX_SOURCE_DIRECTORY(processes bvpl_octree_pro_sources)
AUX_SOURCE_DIRECTORY(processes/pca bvpl_octree_pro_sources)
AUX_SOURCE_DIRECTORY(processes/taylor bvpl_octree_pro_sources)
AUX_SOURCE_DIRECTORY(processes/corners bvpl_octree_pro_sources)
AUX_SOURCE_DIRECTORY(processes/steerable_filters bvpl_octree_pro_sources)


ADD_LIBRARY(bvpl_octree_pro ${bvpl_octree_pro_sources})
TARGET_LINK_LIBRARIES(bvpl_octree_pro bvpl_octree_io bvpl_octree bvpl_kernels_io boxm boct boxm_algo_pro boxm_sample boxm_util bprb brdb vnl vul vbl_io)

IF( BUILD_TESTING )
  SUBDIRS(tests)
ENDIF( BUILD_TESTING )

ENDIF(EXPAT_FOUND)
