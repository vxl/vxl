# brad's Process Library

INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bpro )
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bpro/core )
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bpro/bprb )
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bbas )

SET(brad_pro_sources
    brad_processes.h
    brad_register.h   brad_register.cxx
)
AUX_SOURCE_DIRECTORY(Templates brad_pro_sources)
AUX_SOURCE_DIRECTORY(processes brad_pro_sources)

ADD_LIBRARY(brad_pro ${brad_pro_sources} )

TARGET_LINK_LIBRARIES(brad_pro brad_io brad bprb brdb brip vbl_io vbl vil_pro)

IF( BUILD_TESTING )
  SUBDIRS(tests)
ENDIF( BUILD_TESTING )

INSTALL_TARGETS(/lib brad_pro)
INSTALL_NOBASE_HEADER_FILES(/include/vxl/contrib/brl/bpro/core/brad_pro ${brad_pro_sources})
