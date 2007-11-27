# contrib/brl/bbas/bwm/io/CMakeLists.txt

IF(BUILD_EXPAT)
INCLUDE_DIRECTORIES (${BRL_INCLUDE_DIR}/b3p/expatpp )
#MESSAGE(${BRL_INCLUDE_DIR}/b3p/expatpp )
SET(bwm_io_sources
   bwm_io_config_parser.cxx    bwm_io_config_parser.h
   bwm_io_structs.h
)


ADD_LIBRARY(bwm_io ${bwm_io_sources})

IF ( APPLE )
  TARGET_LINK_LIBRARIES(bwm_io vgl vsl vbl expatpp expat)
ELSE( APPLE )
  TARGET_LINK_LIBRARIES(bwm_io vgl vsl vbl expatpp)
ENDIF ( APPLE )
ENDIF(BUILD_EXPAT)

