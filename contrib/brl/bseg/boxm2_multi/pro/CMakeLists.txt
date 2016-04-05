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


  vxl_add_library(LIBRARY_NAME boxm2_multi_pro LIBRARY_SOURCES ${boxm2_multi_pro_sources})
  #install the .h .hxx and libs

  target_link_libraries(boxm2_multi_pro boxm2_multi_algo bprb)

endif()
