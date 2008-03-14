# brl/bbas/bwm/io/CMakeLists.txt

INCLUDE( ${BRL_MODULE_PATH}/FindEXPAT.cmake )

IF(EXPAT_FOUND)
INCLUDE_DIRECTORIES (${BRL_INCLUDE_DIR}/b3p/expatpp )
#MESSAGE(${BRL_INCLUDE_DIR}/b3p/expatpp )
SET(bwm_io_sources
   bwm_io_config_parser.cxx    bwm_io_config_parser.h
   bwm_io_structs.h
)

ADD_LIBRARY(bwm_io ${bwm_io_sources})

TARGET_LINK_LIBRARIES(bwm_io bwm vgl vsl vsol expatpp)
IF ( APPLE )
  TARGET_LINK_LIBRARIES(bwm_io expat)
ENDIF ( APPLE )
ENDIF(EXPAT_FOUND)

