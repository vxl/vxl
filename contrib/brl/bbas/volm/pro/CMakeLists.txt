# This is contrib/brl/bbas/volm/pro/CMakeLists.txt
# volm Process Library

INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bbas )

SET(volm_pro_sources
    volm_processes.h
    volm_register.h    volm_register.cxx
)

AUX_SOURCE_DIRECTORY(processes volm_pro_sources)

AUX_SOURCE_DIRECTORY(Templates volm_pro_sources)

ADD_LIBRARY(volm_pro ${volm_pro_sources})
TARGET_LINK_LIBRARIES( volm_pro volm bprb brip vul )

IF( BUILD_TESTING )
  SUBDIRS(tests)
ENDIF( BUILD_TESTING )
