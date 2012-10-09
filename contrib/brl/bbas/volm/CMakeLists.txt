# bbas/volm/CMakeLists.txt
# Volumetric Matching and 3-d indexing library

INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bbas/bpgl )
INCLUDE_DIRECTORIES( ${GEL_INCLUDE_DIR} )

INCLUDE( ${BRL_MODULE_PATH}/FindEXPAT.cmake )
IF(EXPAT_FOUND)
 IF(EXPATPP_FOUND)
  INCLUDE_DIRECTORIES (${EXPAT_INCLUDE_DIRS} )
  INCLUDE_DIRECTORIES (${EXPATPP_INCLUDE_DIRS} )
 ENDIF(EXPATPP_FOUND)
ENDIF(EXPAT_FOUND)

SET(volm_sources
 volm_io.h  volm_io.cxx
 volm_spherical_container.h volm_spherical_container.cxx
 volm_query.h volm_query.cxx
 volm_query_sptr.h
)

AUX_SOURCE_DIRECTORY(Templates volm_sources)

ADD_LIBRARY(volm  ${volm_sources})

TARGET_LINK_LIBRARIES(volm bpgl vgl_algo vil vil_io vil_algo vbl_io vsol bkml bvgl vbl vsl vul)

IF(BUILD_TESTING)
  SUBDIRS(tests)
ENDIF(BUILD_TESTING)

SUBDIRS(exe)
