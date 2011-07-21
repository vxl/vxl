# brl/bpro/core/brad_pro/CMakeLists.txt

INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bpro )
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bpro/core )


SET(brad_pro_sources
    brad_processes.h
    brad_register.h             brad_register.cxx
)

AUX_SOURCE_DIRECTORY(Templates brad_pro_sources)
AUX_SOURCE_DIRECTORY(processes brad_pro_sources)

ADD_LIBRARY(brad_pro ${brad_pro_sources})
TARGET_LINK_LIBRARIES(brad_pro bprb brdb brad )
