# contrib/brl/bbas/bwm/io/CMakeLists.txt

INCLUDE_DIRECTORIES( ${VTHREEP_INCLUDE_DIR}/expatpp )

SET(bwm_io_sources
   bwm_io_config_parser.cxx bwm_io_config_parser.h
)

#IF( BUILD_EXAMPLES )
#  SUBDIRS( examples )
#ENDIF( BUILD_EXAMPLES )


ADD_LIBRARY(bwm_io ${bwm_io_sources})

IF ( APPLE )
  TARGET_LINK_LIBRARIES(bwm_io vgl vsl expatpp expat)
ELSE( APPLE )
  TARGET_LINK_LIBRARIES(bwm_io vgl vsl expatpp)
ENDIF ( APPLE )

