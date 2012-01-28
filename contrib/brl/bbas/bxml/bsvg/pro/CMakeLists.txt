# This is contrib/brl/bbas/bxml/bsvg/pro/CMakeLists.txt
# Brown bsvg processes

INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bpro )
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bpro/core )
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bbas )

SET(bsvg_pro_sources
    bsvg_register.h      bsvg_register.cxx
    bsvg_processes.h
   )

AUX_SOURCE_DIRECTORY(Templates bsvg_pro_sources)
AUX_SOURCE_DIRECTORY(processes bsvg_pro_sources)

ADD_LIBRARY(bsvg_pro ${bsvg_pro_sources})
TARGET_LINK_LIBRARIES(bsvg_pro bbas_pro bsvg bprb brdb vbl_io vul)

IF( BUILD_TESTING )
  SUBDIRS(tests)
ENDIF( BUILD_TESTING )
