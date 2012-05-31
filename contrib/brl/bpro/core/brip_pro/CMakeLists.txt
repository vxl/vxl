# brl/bpro/core/brip_pro/CMakeLists.txt

INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bpro )
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bpro/core )
INCLUDE_DIRECTORIES(${GEL_INCLUDE_DIR})

SET(brip_pro_sources
    brip_processes.h
    brip_register.h             brip_register.cxx
)

AUX_SOURCE_DIRECTORY(Templates brip_pro_sources)
AUX_SOURCE_DIRECTORY(processes brip_pro_sources)

ADD_LIBRARY(brip_pro ${brip_pro_sources})
TARGET_LINK_LIBRARIES(brip_pro bprb brdb brip vil_pro vil vsl bil_algo)

IF( BUILD_TESTING )
  SUBDIRS(tests)
ENDIF( BUILD_TESTING )
