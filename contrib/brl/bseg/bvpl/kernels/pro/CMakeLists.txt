# This is contrib/brl/bseg/bvpl/kernels/pro/CMakeLists.txt
# BVPL kenel's related processes

# bprb depends on expat and bvxm_pro depends on bprb primarily
INCLUDE( ${BRL_MODULE_PATH}/FindEXPAT.cmake )
IF(EXPAT_FOUND)
  INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bpro )
  INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bpro/bprb )
  INCLUDE_DIRECTORIES( ${GEL_INCLUDE_DIR}/mrc )
  INCLUDE_DIRECTORIES( ${MUL_INCLUDE_DIR} )
  SET(bvpl_kernels_pro_sources
      bvpl_kernels_processes.h
      bvpl_kernels_register.h    bvpl_kernels_register.cxx
     )

  AUX_SOURCE_DIRECTORY(Templates bvpl_kernels_pro_sources)
  AUX_SOURCE_DIRECTORY(processes bvpl_kernels_pro_sources)

  ADD_LIBRARY(bvpl_kernels_pro ${bvpl_kernels_pro_sources})

  TARGET_LINK_LIBRARIES(bvpl_kernels_pro bvpl_kernels bvpl_kernels_io bvpl_octree bvpl_octree_io bsta bprb brdb bvpl vnl vul vbl_io)

  IF( BUILD_TESTING )
    SUBDIRS(tests)
  ENDIF( BUILD_TESTING )

ENDIF(EXPAT_FOUND)
