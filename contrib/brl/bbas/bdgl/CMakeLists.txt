# Brown digital geometry library
# brl/bbas/bdgl/CMakeLists.txt
INCLUDE_DIRECTORIES(${BRL_INCLUDE_DIR}/bbas )
INCLUDE_DIRECTORIES(${GEL_INCLUDE_DIR} )

SET(bdgl_sources
    bdgl_curve_algs.cxx   bdgl_curve_algs.h
    bdgl_region_algs.cxx   bdgl_region_algs.h
    bdgl_curve_region.cxx bdgl_curve_region.h
)
AUX_SOURCE_DIRECTORY(Templates bdgl_sources)

ADD_LIBRARY(bdgl ${bdgl_sources})

TARGET_LINK_LIBRARIES(bdgl btol vdgl vsol vgl_algo vgl vnl_algo vnl)

IF(BUILD_TESTING)
  SUBDIRS(tests)
ENDIF(BUILD_TESTING)
