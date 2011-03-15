# bsta's Process Library

INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bpro )
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bpro/bprb )
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bbas )

SET(bsta_pro_sources
    bsta_processes.h
    bsta_register.h   bsta_register.cxx
)

AUX_SOURCE_DIRECTORY(Templates bsta_pro_sources)
AUX_SOURCE_DIRECTORY(processes bsta_pro_sources)

ADD_LIBRARY(bsta_pro ${bsta_pro_sources} )

TARGET_LINK_LIBRARIES(bsta_pro bsta bprb brdb brip vbl_io)

IF( BUILD_TESTING )
  SUBDIRS(tests)
ENDIF( BUILD_TESTING )
