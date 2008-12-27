# brl/bpro/core/brip_pro/CMakeLists.txt


INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bpro )
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bpro/core )
INCLUDE_DIRECTORIES(${GEL_INCLUDE_DIR})
SET(brip_pro_sources
    brip_extrema_process.h   brip_extrema_process.cxx
)

AUX_SOURCE_DIRECTORY(Templates brip_pro_sources)

ADD_LIBRARY(brip_pro ${brip_pro_sources})
TARGET_LINK_LIBRARIES(brip_pro bprb brdb brip vsl)
