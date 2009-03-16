# This is contrib/brl/bseg/bvxm/pro/CMakeLists.txt
# bvxm's Process Library

# bprb depends on expat and bvxm_pro depends on bprb primarily
INCLUDE( ${BRL_MODULE_PATH}/FindEXPAT.cmake )
IF(EXPAT_FOUND)
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bpro )
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bpro/bprb )
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bseg )
INCLUDE_DIRECTORIES( ${GEL_INCLUDE_DIR}/mrc )
INCLUDE_DIRECTORIES( ${GEL_INCLUDE_DIR})
INCLUDE(${MODULE_PATH}/FindTIFF.cmake)
IF(TIFF_FOUND)
  INCLUDE_DIRECTORIES(${TIFF_INCLUDE_DIR})
  INCLUDE( ${MODULE_PATH}/FindGEOTIFF.cmake)
  IF(GEOTIFF_FOUND)
    INCLUDE_DIRECTORIES(${GEOTIFF_INCLUDE_DIR})
  ENDIF(GEOTIFF_FOUND)
ENDIF(TIFF_FOUND)

SET(bvxm_pro_sources
    bvxm_processes.h
    bvxm_register.h    bvxm_register.cxx
   )

AUX_SOURCE_DIRECTORY(Templates bvxm_pro_sources)
AUX_SOURCE_DIRECTORY(processes bvxm_pro_sources)
SET(bvxm_pro_sources ${bvxm_pro_sources}
    processes/bvxm_normalization_util.h
   )

ADD_LIBRARY(bvxm_pro ${bvxm_pro_sources})

TARGET_LINK_LIBRARIES(bvxm_pro bprb bvxm bvxm_io sdet vpgl vpgl_io vpgl_algo vpgl_file_formats vpgl_pro vsol vil_io vil_pro vnl_io bgeo brdb)

IF( BUILD_TESTING )
  SUBDIRS(tests)
ENDIF( BUILD_TESTING )
ENDIF(EXPAT_FOUND)
