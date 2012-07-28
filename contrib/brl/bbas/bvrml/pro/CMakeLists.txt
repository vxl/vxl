# bsta's Process Library

INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bpro )
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bpro/bprb )
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bbas )
INCLUDE_DIRECTORIES(${VXLCORE_INCLUDE_DIR})
INCLUDE_DIRECTORIES( ${VTHREEP_INCLUDE_DIR}/rply )

SET(bvrml_pro_sources
    bvrml_processes.h
    bvrml_register.h   bvrml_register.cxx
)

AUX_SOURCE_DIRECTORY(processes bvrml_pro_sources)
AUX_SOURCE_DIRECTORY(Templates bvrml_pro_sources)

ADD_LIBRARY(bvrml_pro ${bvrml_pro_sources} )

TARGET_LINK_LIBRARIES(bvrml_pro bvrml vpgl_algo vpgl_io bprb rply )

IF( BUILD_TESTING )
  SUBDIRS(tests)
ENDIF( BUILD_TESTING )

INSTALL_TARGETS(/lib bvrml_pro)
INSTALL_NOBASE_HEADER_FILES(/include/vxl/contrib/brl/bbas/bvrml/pro ${bvrml_pro_sources})
