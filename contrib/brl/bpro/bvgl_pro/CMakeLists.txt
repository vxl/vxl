# brl/bpro/bvgl_pro/CMakeLists.txt

INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bpro )
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bpro/core )
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bseg )
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bbas )
INCLUDE_DIRECTORIES( ${GEL_INCLUDE_DIR} )

SET(bvgl_pro_sources
    bvgl_generate_mask_process.h  bvgl_generate_mask_process.cxx
)

#AUX_SOURCE_DIRECTORY(Templates bvgl_pro_sources)

ADD_LIBRARY(bvgl_pro ${bvgl_pro_sources})

TARGET_LINK_LIBRARIES(bvgl_pro bvgl vil vil_io)

#IF( BUILD_TESTING )
#  SUBDIRS(tests)
#ENDIF( BUILD_TESTING )
