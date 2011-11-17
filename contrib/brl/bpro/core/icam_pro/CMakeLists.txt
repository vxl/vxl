# brl/bpro/core/icam_pro/CMakeLists.txt
# Brown vpgl processes

INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bpro )
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bpro/core )
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bbas/bpgl )
INCLUDE_DIRECTORIES( ${GEL_INCLUDE_DIR}/mrc )

SET( icam_pro_sources
  icam_processes.h
  icam_register.h      icam_register.cxx
)
AUX_SOURCE_DIRECTORY(Templates icam_pro_sources)
AUX_SOURCE_DIRECTORY(processes icam_pro_sources)

ADD_LIBRARY(icam_pro ${icam_pro_sources})
TARGET_LINK_LIBRARIES(icam_pro bprb icam vpgl_algo vpgl_io vpgl vnl vil_io vil vbl vsl)

IF( BUILD_TESTING )
  SUBDIRS(tests)
ENDIF( BUILD_TESTING )
