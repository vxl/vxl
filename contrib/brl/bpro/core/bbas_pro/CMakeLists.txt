# brl/bpro/core/bbas_pro/CMakeLists.txt

INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bpro )
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bpro/core )
INCLUDE_DIRECTORIES( ${GEL_INCLUDE_DIR})

SET(bbas_pro_sources
    bbas_processes.h
    bbas_register.h             bbas_register.cxx
    bbas_1d_array_float.h       bbas_1d_array_float.cxx
   )

AUX_SOURCE_DIRECTORY(Templates bbas_pro_sources)
AUX_SOURCE_DIRECTORY(processes bbas_pro_sources)

ADD_LIBRARY(bbas_pro ${bbas_pro_sources})
TARGET_LINK_LIBRARIES(bbas_pro bprb brdb  brad bsta vbl_io vbl vsl)
