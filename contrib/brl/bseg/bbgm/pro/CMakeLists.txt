# brl/bseg/bbgm/pro/CMakeLists.txt

INCLUDE( ${BRL_MODULE_PATH}/FindEXPAT.cmake )
IF(EXPAT_FOUND)

INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bseg )
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bpro )
INCLUDE_DIRECTORIES( ${GEL_INCLUDE_DIR} )
SET(bbgm_pro_sources
    bbgm_register.h                   bbgm_register.cxx
    bbgm_processes.h
)

AUX_SOURCE_DIRECTORY(Templates bbgm_pro_sources)
AUX_SOURCE_DIRECTORY(processes bbgm_pro_sources)

ADD_LIBRARY(bbgm_pro ${bbgm_pro_sources})
TARGET_LINK_LIBRARIES(bbgm_pro bbgm bprb brdb bsta_algo bsta brip vidl vil vil_pro vbl_io vsl)

IF( BUILD_TESTING )
  SUBDIRS(tests)
ENDIF( BUILD_TESTING )

ENDIF(EXPAT_FOUND)
