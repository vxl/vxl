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

#AUX_SOURCE_DIRECTORY(Templates bvpl_pro_sources)
AUX_SOURCE_DIRECTORY(processes bvpl_octree_pro_sources)

ADD_LIBRARY(bvpl_octree_pro ${bvpl_octree_pro_sources})

TARGET_LINK_LIBRARIES(bvpl_octree_pro bvpl_octree boxm boct boxm_opt bprb bvpl_io brdb)

IF( BUILD_TESTING )
  SUBDIRS(tests)
ENDIF( BUILD_TESTING )

ENDIF(EXPAT_FOUND)
