# ./brl/bbas/bugl/CMakeLists.txt
# writen by Kongbin Kang for creating a brown uncertainty geometry library.

INCLUDE_DIRECTORIES(${BRL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES(${BRL_INCLUDE_DIR}/bbas)

SET(bugl_sources
    bugl_uncertainty_point_2d.h  bugl_uncertainty_point_2d.txx
    bugl_gaussian_point_2d.h     bugl_gaussian_point_2d.txx
    bugl_gaussian_point_2d_sptr.h
    
    bugl_uncertainty_point_3d.h bugl_uncertainty_point_3d.txx
    bugl_gaussian_point_3d.h     bugl_gaussian_point_3d.txx
    bugl_gaussian_point_3d_sptr.h
)

AUX_SOURCE_DIRECTORY(Templates bugl_sources)

ADD_LIBRARY(bugl ${bugl_sources})

TARGET_LINK_LIBRARIES(bugl vnl vgl)

IF(BUILD_TESTING)
  SUBDIRS(tests)
ENDIF(BUILD_TESTING)

