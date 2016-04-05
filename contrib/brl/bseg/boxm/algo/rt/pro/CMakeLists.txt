# boxm's Process Library

# bprb depends on expat and boxm_pro depends on bprb primarily
include( ${BRL_MODULE_PATH}/FindEXPAT.cmake )
if(EXPAT_FOUND)
  include_directories( ${BRL_INCLUDE_DIR}/bpro )
  include_directories( ${BRL_INCLUDE_DIR}/bpro/bprb )
  include_directories( ${BRL_INCLUDE_DIR}/bseg )
 # include_directories( ${GEL_INCLUDE_DIR}/mrc/vpgl )
 # include_directories( ${GEL_INCLUDE_DIR}/mrc )

  set(boxm_algo_rt_pro_sources
      boxm_algo_rt_processes.h
      boxm_algo_rt_register.h   boxm_algo_rt_register.cxx
  )

  #aux_source_directory(Templates boxm_algo_rt_pro_sources)
  aux_source_directory(processes boxm_algo_rt_pro_sources)

  vxl_add_library(LIBRARY_NAME boxm_algo_rt_pro LIBRARY_SOURCES ${boxm_algo_rt_pro_sources} )

  target_link_libraries(boxm_algo_rt_pro boxm_algo_rt boxm_algo_pro boxm boxm_sample boxm_sample_algo boct bprb brdb brip sdet ${VXL_LIB_PREFIX}vgl ${VXL_LIB_PREFIX}vnl_algo ${VXL_LIB_PREFIX}vil ${VXL_LIB_PREFIX}vul ${VXL_LIB_PREFIX}vcl)

  if( BUILD_TESTING )
    add_subdirectory(tests)
  endif()

endif()
