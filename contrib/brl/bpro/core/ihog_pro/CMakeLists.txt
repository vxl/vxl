# brl/bpro/core/ihog_pro/CMakeLists.txt
# Brown ihog processes

INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bpro )
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bpro/core )
INCLUDE_DIRECTORIES( ${GEL_INCLUDE_DIR}/mrc )
INCLUDE_DIRECTORIES( ${GEL_INCLUDE_DIR}/mrc/vpgl )
INCLUDE_DIRECTORIES( ${GEL_INCLUDE_DIR}/mrc/vpgl/ihog )

SET( ihog_pro_sources
  ihog_processes.h
  ihog_register.h                          ihog_register.cxx
)

AUX_SOURCE_DIRECTORY(Templates ihog_pro_sources)

AUX_SOURCE_DIRECTORY(processes ihog_pro_sources)

ADD_LIBRARY(ihog_pro ${ihog_pro_sources})

TARGET_LINK_LIBRARIES(ihog_pro bprb ihog vnl vil vbl_io)
