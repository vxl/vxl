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
IF(OPENCL_FOUND)
  INCLUDE_DIRECTORIES( ${OPENCL_INCLUDE_PATH} )
ENDIF(OPENCL_FOUND)

  SET(boxm2_volm_pro_sources
      boxm2_volm_processes.h
      boxm2_volm_register.h   boxm2_volm_register.cxx
     )

  AUX_SOURCE_DIRECTORY(Templates boxm2_volm_pro_sources)
IF(OPENCL_FOUND)
  AUX_SOURCE_DIRECTORY(processes boxm2_volm_pro_sources)
ENDIF(OPENCL_FOUND)

  ADD_LIBRARY(boxm2_volm_pro ${boxm2_volm_pro_sources})

  #install the .h .txx and libs
  INSTALL_TARGETS(/lib boxm2_volm_pro)
  INSTALL_NOBASE_HEADER_FILES(/include/vxl/contrib/brl/bseg/boxm2/volm/pro ${boxm2_volm_pro_sources})

  TARGET_LINK_LIBRARIES(boxm2_volm_pro boxm2_volm boxm2_volm_io boxm2_io boxm2_util boxm2_cpp_algo bbas_pro bvgl sdet bprb brdb brip vsol bsta bsta_pro vpgl_pro vnl vil_algo vil_io vil vgl vbl_io vul bvrml)

ENDIF(EXPAT_FOUND)

#tests
IF( BUILD_TESTING )
  SUBDIRS(tests)
ENDIF( BUILD_TESTING )
