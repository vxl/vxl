# This is contrib/brl/bseg/bmdl/pro/CMakeLists.txt
# bmdl's Process Library

# bprb depends on expat and bmdl_prodepends on bprb primarily
INCLUDE( ${BRL_MODULE_PATH}/FindEXPAT.cmake )
IF(EXPAT_FOUND)
  INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bpro )
  INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bseg )
  INCLUDE_DIRECTORIES( ${GEL_INCLUDE_DIR}/mrc )
  INCLUDE_DIRECTORIES( ${GEL_INCLUDE_DIR}/mrc/vpgl )
  INCLUDE_DIRECTORIES( ${GEL_INCLUDE_DIR})
  INCLUDE_DIRECTORIES( ${vxl_SOURCE_DIR}/v3p/tiff )
  INCLUDE_DIRECTORIES( ${vxl_SOURCE_DIR}/v3p/geotiff )
  SET(bmdl_pro_sources
    bmdl_classify_process.h               bmdl_classify_process.cxx
    bmdl_trace_boundaries_process.h       bmdl_trace_boundaries_process.cxx
    bmdl_generate_mesh_process.h          bmdl_generate_mesh_process.cxx
    bmdl_lidar_roi_process.h              bmdl_lidar_roi_process.cxx
  )

  AUX_SOURCE_DIRECTORY(Templates bmdl_pro_sources)

  ADD_LIBRARY(bmdl_pro ${bmdl_pro_sources})

  TARGET_LINK_LIBRARIES(bmdl_pro bmdl bprb brdb vil vil_io bgeo vgl vgl_io vpgl
  vpgl_pro bvxm bgeo vsol brip)

  IF( BUILD_TESTING )
    SUBDIRS(tests)
  ENDIF( BUILD_TESTING )
ENDIF(EXPAT_FOUND)
