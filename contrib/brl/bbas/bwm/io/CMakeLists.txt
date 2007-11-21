# contrib/brl/bbas/bwm/io/CMakeLists.txt

INCLUDE_DIRECTORIES (${BRL_INCLUDE_DIR}/b3p/expatpp )

SET(bwm_io_sources
   bwm_io_config_parser.cxx bwm_io_config_parser.h
   bwm_io_structs.h
)

ADD_LIBRARY(bwm_io ${bwm_io_sources})

IF ( APPLE )
  TARGET_LINK_LIBRARIES(bwm_io vgl vsl expatpp expat)
ELSE( APPLE )
  TARGET_LINK_LIBRARIES(bwm_io vgl vsl expatpp)
ENDIF ( APPLE )

