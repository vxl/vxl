# boxm2 multi gpu or cpu Process Library

# bprb depends on expat and boxm2_multi_pro depends on bprb primarily
include( ${BRL_MODULE_PATH}/FindEXPAT.cmake )
if(EXPAT_FOUND)
  include_directories( ${BRL_INCLUDE_DIR}/bseg )
  set(boxm2_multi_pro_sources
      boxm2_multi_processes.h
      boxm2_multi_register.h   boxm2_multi_register.cxx
     )

  aux_source_directory(Templates boxm2_multi_pro_sources)
  aux_source_directory(processes boxm2_multi_pro_sources)


  add_library(boxm2_multi_pro ${boxm2_multi_pro_sources})
  #install the .h .txx and libs
  install_targets(/lib boxm2_multi_pro)
  INSTALL_NOBASE_HEADER_FILES(/include/vxl/contrib/brl/bseg/boxm2_multi/pro ${boxm2_multi_pro_sources})

  target_link_libraries(boxm2_multi_pro boxm2_multi_algo bprb)

endif(EXPAT_FOUND)
