#This is boxm2/volm/conf/CMakeLists.txt
# volumetric configurational matcher library

INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bbas/volm )

SET(boxm2_volm_conf_sources
  boxm2_volm_conf_matcher.h       boxm2_volm_conf_matcher.cxx
)

ADD_LIBRARY(boxm2_volm_conf ${boxm2_volm_conf_sources})

TARGET_LINK_LIBRARIES(boxm2_volm_conf boxm2_volm boxm2 boxm2_io volm volm_conf vul vgl vcl)

IF(BUILD_TESTING)
  SUBDIRS(tests)
ENDIF(BUILD_TESTING)

SUBDIRS(exe)