# Brown basic geometry library
# brl/bbas/bvgl/CMakeLists.txt
INCLUDE_DIRECTORIES(${GEL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES(${BRL_INCLUDE_DIR}/bbas )

SET(bvgl_sources
bvgl_h_matrix_1d.h bvgl_h_matrix_1d.txx 
bvgl_h_matrix_2d.h bvgl_h_matrix_2d.txx 
)

AUX_SOURCE_DIRECTORY(Templates bvgl_sources)

ADD_LIBRARY(bvgl ${bvgl_sources})

TARGET_LINK_LIBRARIES(bvgl vgl vnl vcl)

IF( BUILD_TESTING )
  SUBDIRS(tests)
ENDIF( BUILD_TESTING )

