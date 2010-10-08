# brl/bpro/core/icam_pro/CMakeLists.txt
# Brown vpgl processes

INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bpro )
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bpro/core )
INCLUDE_DIRECTORIES( ${GEL_INCLUDE_DIR}/mrc )
INCLUDE_DIRECTORIES( ${GEL_INCLUDE_DIR}/mrc/vpgl )

SET( icam_pro_sources
  icam_processes.h
  icam_register.h                          icam_register.cxx
)

AUX_SOURCE_DIRECTORY(Templates icam_pro_sources)

AUX_SOURCE_DIRECTORY(processes icam_pro_sources)

ADD_LIBRARY(icam_pro ${icam_pro_sources})

TARGET_LINK_LIBRARIES(icam_pro bprb icam vpgl vpgl_algo vnl vil)

