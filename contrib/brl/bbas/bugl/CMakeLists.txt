# ./brl/bbas/bugl/CMakeLists.txt

INCLUDE_DIRECTORIES(${BRL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES(${BRL_INCLUDE_DIR}/bbas)

SET(bugl_sources
    bugl_uncertainty_point_2d.h  bugl_uncertainty_point_2d.txx
    bugl_gaussian_point_2d.h     bugl_gaussian_point_2d.txx
)

AUX_SOURCE_DIRECTORY(Templates bugl_sources)

ADD_LIBRARY(bugl ${bugl_sources})

TARGET_LINK_LIBRARIES(bugl vnl vgl)

IF(BUILD_TESTING)
  SUBDIRS(tests)
ENDIF(BUILD_TESTING)

