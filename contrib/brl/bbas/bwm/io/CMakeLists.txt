# brl/bbas/bwm/io/CMakeLists.txt

SET(bwm_io_sources
   bwm_io_structs.h
   bwm_site.h                  bwm_site.cxx
   bwm_site_sptr.h
)
AUX_SOURCE_DIRECTORY(Templates bwm_io_sources)

INCLUDE( ${BRL_MODULE_PATH}/FindEXPAT.cmake )

IF(EXPAT_FOUND)
 IF(EXPATPP_FOUND)
  INCLUDE_DIRECTORIES (${EXPAT_INCLUDE_DIRS} )
  INCLUDE_DIRECTORIES (${EXPATPP_INCLUDE_DIRS} )
  SET(bwm_io_sources ${bwm_io_sources}
     bwm_io_config_parser.cxx    bwm_io_config_parser.h
  )
 ENDIF(EXPATPP_FOUND)
ENDIF(EXPAT_FOUND)

ADD_LIBRARY(bwm_io ${bwm_io_sources})

TARGET_LINK_LIBRARIES(bwm_io vsol vsl vbl)

IF(EXPAT_FOUND)
  TARGET_LINK_LIBRARIES(bwm_io ${EXPAT_LIBRARIES})
  IF(APPLE)
    TARGET_LINK_LIBRARIES(bwm_io expat)
  ENDIF(APPLE)
  IF(EXPATPP_FOUND)
    TARGET_LINK_LIBRARIES(bwm_io ${EXPATPP_LIBRARIES})
  ENDIF(EXPATPP_FOUND)
ENDIF(EXPAT_FOUND)
