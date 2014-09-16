# boxm2 multi gpu or cpu Process Library

# bprb depends on expat and boxm2_multi_pro depends on bprb primarily
INCLUDE( ${BRL_MODULE_PATH}/FindEXPAT.cmake )
IF(EXPAT_FOUND)
  INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bseg )
  SET(boxm2_multi_pro_sources
      boxm2_multi_processes.h
      boxm2_multi_register.h   boxm2_multi_register.cxx
     )

  AUX_SOURCE_DIRECTORY(Templates boxm2_multi_pro_sources)
  AUX_SOURCE_DIRECTORY(processes boxm2_multi_pro_sources)


  ADD_LIBRARY(boxm2_multi_pro ${boxm2_multi_pro_sources})
  #install the .h .txx and libs
  INSTALL_TARGETS(/lib boxm2_multi_pro)
  INSTALL_NOBASE_HEADER_FILES(/include/vxl/contrib/brl/bseg/boxm2_multi/pro ${boxm2_multi_pro_sources})

  TARGET_LINK_LIBRARIES(boxm2_multi_pro boxm2_multi_algo)

ENDIF(EXPAT_FOUND)
