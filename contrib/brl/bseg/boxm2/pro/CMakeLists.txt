# boxm2's Process Library

# bprb depends on expat and boxm2_pro depends on bprb primarily
INCLUDE( ${BRL_MODULE_PATH}/FindEXPAT.cmake )
IF(EXPAT_FOUND)
  INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bpro/core )
  INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bpro )
  INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bpro/bprb )
  INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bseg )
  INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bbas )
  INCLUDE_DIRECTORIES( ${GEL_INCLUDE_DIR}/mrc/vpgl )
  INCLUDE_DIRECTORIES( ${GEL_INCLUDE_DIR}/mrc )
  INCLUDE_DIRECTORIES( ${VTHREEP_INCLUDE_DIR}/rply )
  
  INCLUDE(${VXL_CMAKE_DIR}/FindTIFF.cmake)
  IF(TIFF_FOUND)
    INCLUDE_DIRECTORIES(${TIFF_INCLUDE_DIR})
    INCLUDE( ${VXL_CMAKE_DIR}/FindGEOTIFF.cmake)
    IF(GEOTIFF_FOUND)
      INCLUDE_DIRECTORIES(${GEOTIFF_INCLUDE_DIR})
      ADD_DEFINITIONS( -DHAS_GEOTIFF=1 )
    ELSE(GEOTIFF_FOUND)
      ADD_DEFINITIONS( -DHAS_GEOTIFF=0 )
    ENDIF(GEOTIFF_FOUND)
  ENDIF(TIFF_FOUND)
  
 IF(lems_SOURCE_DIR)
  INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/contrib/dec )
 ENDIF(lems_SOURCE_DIR)

  IF(lems_SOURCE_DIR)
    INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/contrib/dec )
  ENDIF(lems_SOURCE_DIR)

  SET(boxm2_pro_sources
      boxm2_processes.h
      boxm2_register.h   boxm2_register.cxx
     )

  AUX_SOURCE_DIRECTORY(Templates boxm2_pro_sources)
  AUX_SOURCE_DIRECTORY(processes boxm2_pro_sources)

  INCLUDE(${VXL_CMAKE_DIR}/FindTIFF.cmake)
  IF(TIFF_FOUND)
    INCLUDE_DIRECTORIES(${TIFF_INCLUDE_DIR})
    SET( HAS_TIFF 1 )
  ENDIF(TIFF_FOUND)
  INCLUDE( ${VXL_CMAKE_DIR}/FindGEOTIFF.cmake)
  IF(GEOTIFF_FOUND)
    INCLUDE_DIRECTORIES(${GEOTIFF_INCLUDE_DIR})
    SET (HAS_GEOTIFF 1 )
  ENDIF(GEOTIFF_FOUND)

  ADD_LIBRARY(boxm2_pro ${boxm2_pro_sources})

  #install the .h .txx and libs
  INSTALL_TARGETS(/lib boxm2_pro)
  INSTALL_NOBASE_HEADER_FILES(/include/vxl/contrib/brl/bseg/boxm2/pro ${boxm2_pro_sources})

  TARGET_LINK_LIBRARIES(boxm2_pro boxm2 boxm2_io boxm2_util boxm2_cpp_algo bbas_pro bvgl sdet bprb brdb brip imesh vsol bsta bsta_pro vpgl_pro vnl vil_algo vil_io vil vgl vbl_io vul bvrml bmsh3d_algo rply volm)

  IF( BUILD_TESTING )
    SUBDIRS(tests)
  ENDIF( BUILD_TESTING )

ENDIF(EXPAT_FOUND)
