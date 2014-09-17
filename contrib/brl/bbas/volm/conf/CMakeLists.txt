# volm/conf/CMakeLists.txt
# 2d/3d configuration matching and base library

INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bbas )

INCLUDE( ${BRL_MODULE_PATH}/FindEXPAT.cmake )
IF(EXPAT_FOUND)
 IF(EXPATPP_FOUND)
  INCLUDE_DIRECTORIES (${EXPAT_INCLUDE_DIRS} )
  INCLUDE_DIRECTORIES (${EXPATPP_INCLUDE_DIRS} )
 ENDIF(EXPATPP_FOUND)
ENDIF(EXPAT_FOUND)

SET (volm_conf_sources
  volm_conf_object.h              volm_conf_object.cxx
  volm_conf_query.h               volm_conf_query.cxx
  volm_conf_land_map_indexer.h    volm_conf_land_map_indexer.cxx
  volm_conf_buffer.h              volm_conf_buffer.txx
  volm_conf_indexer.h             volm_conf_indexer.cxx
  volm_conf_2d_indexer.h          volm_conf_2d_indexer.cxx
  volm_conf_score.h               volm_conf_score.cxx
)

AUX_SOURCE_DIRECTORY(Templates volm_conf_sources)

ADD_LIBRARY(volm_conf ${volm_conf_sources})

TARGET_LINK_LIBRARIES(volm_conf volm vgl_io vil vil_io vbl_io bkml bvgl bpgl vpgl vpgl_io vsol vbl vsl vul vnl bvrml depth_map vdgl sdet vtol bsol bvgl_algo)

IF(BUILD_TESTING)
  SUBDIRS(tests)
ENDIF(BUILD_TESTING)

SUBDIRS(exe)
