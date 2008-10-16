# This is contrib/brl/bseg/bmdl/pro/CMakeLists.txt
# bmdl's Process Library

# bprb depends on expat and bmdl_prodepends on bprb primarily
INCLUDE( ${BRL_MODULE_PATH}/FindEXPAT.cmake )
IF(EXPAT_FOUND)
  INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bpro )
  INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bseg )
  INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bbas/dbil/algo )
  INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/b3p/shapelib)
  INCLUDE_DIRECTORIES( ${GEL_INCLUDE_DIR}/mrc )
  INCLUDE_DIRECTORIES( ${GEL_INCLUDE_DIR})

  SET(bmdl_pro_sources

    bmdl_classify_process.h               bmdl_classify_process.cxx
    bmdl_trace_boundaries_process.h       bmdl_trace_boundaries_process.cxx
    bmdl_generate_mesh_process.h          bmdl_generate_mesh_process.cxx
  )

  AUX_SOURCE_DIRECTORY(Templates bmdl_pro_sources)

  ADD_LIBRARY(bmdl_pro ${bmdl_pro_sources})

  TARGET_LINK_LIBRARIES(bmdl_pro bmdl bprb bil_algo sdet vpgl vpgl_io vpgl_algo vpgl_file_formats vpgl_pro vsol vil vil_io vil_pro vnl vnl_io bgeo vgl vgl_algo brip brdb)

  IF( BUILD_TESTING )
    SUBDIRS(tests)
  ENDIF( BUILD_TESTING )
ENDIF(EXPAT_FOUND)
