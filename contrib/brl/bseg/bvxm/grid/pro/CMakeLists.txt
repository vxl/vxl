# This is contrib/brl/bseg/bvxm/pro/CMakeLists.txt
# bvxm's Process Library

# bprb depends on expat and bvxm_pro depends on bprb primarily
INCLUDE( ${BRL_MODULE_PATH}/FindEXPAT.cmake )
IF(EXPAT_FOUND)
  INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bpro )
  INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bpro/bprb )
  INCLUDE(${VXL_CMAKE_DIR}/FindTIFF.cmake)
  IF(TIFF_FOUND)
    INCLUDE_DIRECTORIES(${TIFF_INCLUDE_DIR})
    INCLUDE( ${VXL_CMAKE_DIR}/FindGEOTIFF.cmake)
    IF(GEOTIFF_FOUND)
      INCLUDE_DIRECTORIES(${GEOTIFF_INCLUDE_DIR})
    ENDIF(GEOTIFF_FOUND)
  ENDIF(TIFF_FOUND)

  SET(bvxm_grid_pro_sources
      bvxm_grid_processes.h
      bvxm_grid_register.h    bvxm_grid_register.cxx
     )
  AUX_SOURCE_DIRECTORY(Templates bvxm_grid_pro_sources)
  AUX_SOURCE_DIRECTORY(processes bvxm_grid_pro_sources)

  SET(bvxm_grid_pro_sources ${bvxm_grid_pro_sources})

  ADD_LIBRARY(bvxm_grid_pro ${bvxm_grid_pro_sources})

  TARGET_LINK_LIBRARIES(bvxm_grid_pro bvxm_grid bvxm_grid_io bprb brdb vul vgl imesh vnl vbl_io)

  IF( BUILD_TESTING )
    SUBDIRS(tests)
  ENDIF( BUILD_TESTING )

ENDIF(EXPAT_FOUND)
