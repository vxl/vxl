# This is contrib/brl/bbas/bwm/pro/CMakeLists.txt
# bwm's Process Library

# bwm depends on expat and bwm_pro depends on bwm primarily
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bbas )

SET(bwm_pro_sources
    bwm_processes.h
    bwm_register.h    bwm_register.cxx
)

INCLUDE( ${BRL_MODULE_PATH}/FindEXPAT.cmake )

IF(EXPAT_FOUND)
 IF(EXPATPP_FOUND)
  INCLUDE_DIRECTORIES (${EXPAT_INCLUDE_DIRS} )
  INCLUDE_DIRECTORIES (${EXPATPP_INCLUDE_DIRS} )
  SET(bwm_pro_sources ${bwm_pro_sources}
      processes/bwm_create_corr_file_process.cxx
     )
 ENDIF(EXPATPP_FOUND)
ENDIF(EXPAT_FOUND)

AUX_SOURCE_DIRECTORY(Templates bwm_pro_sources)

ADD_LIBRARY(bwm_pro ${bwm_pro_sources})
TARGET_LINK_LIBRARIES( bwm_pro bprb brip bwm_io vul bwm_algo)

IF( BUILD_TESTING )
  SUBDIRS(tests)
ENDIF( BUILD_TESTING )
