# Brown digital geometry library
# brl/bbas/bdgl/CMakeLists.txt
INCLUDE_DIRECTORIES(${GEL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES(${BRL_INCLUDE_DIR}/bbas )

SET(bdgl_sources
    bdgl_curve_algs.cxx     bdgl_curve_algs.h
)
AUX_SOURCE_DIRECTORY(Templates bdgl_sources)

ADD_LIBRARY(bdgl ${bdgl_sources})

TARGET_LINK_LIBRARIES(bdgl vdgl vtol vsol vil vnl vbl)

IF(BUILD_TESTING)
  SUBDIRS(tests)
ENDIF(BUILD_TESTING)
