# brl/bbas/bkml/CMakeLists.txt
# KML utilities for Brown University Libraries

SET(bkml_sources
  bkml_write.h bkml_write.cxx
)

INCLUDE( ${BRL_MODULE_PATH}/FindEXPAT.cmake )

IF(EXPAT_FOUND)
 IF(EXPATPP_FOUND)
  INCLUDE_DIRECTORIES (${EXPAT_INCLUDE_DIRS} )
  INCLUDE_DIRECTORIES (${EXPATPP_INCLUDE_DIRS} )
  SET(bkml_sources ${bkml_sources}
     bkml_parser.cxx    bkml_parser.h
  )
 ENDIF(EXPATPP_FOUND)
ENDIF(EXPAT_FOUND)

ADD_LIBRARY(bkml  ${bkml_sources})

TARGET_LINK_LIBRARIES(bkml vgl)

IF(EXPAT_FOUND)
  TARGET_LINK_LIBRARIES(bkml ${EXPAT_LIBRARIES})
  IF(APPLE)
    TARGET_LINK_LIBRARIES(bkml expat)
  ENDIF(APPLE)
  IF(EXPATPP_FOUND)
    TARGET_LINK_LIBRARIES(bkml ${EXPATPP_LIBRARIES})
  ENDIF(EXPATPP_FOUND)
ENDIF(EXPAT_FOUND)

IF( BUILD_TESTING )
  SUBDIRS(tests)
ENDIF( BUILD_TESTING )

#SUBDIRS(pro)

INSTALL_TARGETS(/lib bkml)
INSTALL_NOBASE_HEADER_FILES(/include/vxl/contrib/brl/bbas/bkml ${bkml_sources})