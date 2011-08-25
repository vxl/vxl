# This is contrib/brl/bseg/bapl/pro/CMakeLists.txt
# Brown Affine Patch Process Library

INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bpro )
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bseg )
INCLUDE_DIRECTORIES( ${GEL_INCLUDE_DIR}/mrc/vpgl )
INCLUDE_DIRECTORIES( ${GEL_INCLUDE_DIR}/mrc )
INCLUDE_DIRECTORIES( ${GEL_INCLUDE_DIR})
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bseg/bvxm )
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bseg/brec )

SET(bapl_pro_sources
    bapl_register.h      bapl_register.cxx
    bapl_processes.h
   )

AUX_SOURCE_DIRECTORY(Templates bapl_pro_sources)
AUX_SOURCE_DIRECTORY(processes bapl_pro_sources)

ADD_LIBRARY(bapl_pro ${bapl_pro_sources})
TARGET_LINK_LIBRARIES(bapl_pro bapl bapl_io bprb brdb vil_pro vil_io vil vnl vul)

IF( BUILD_TESTING )
  SUBDIRS(tests)
ENDIF( BUILD_TESTING )
