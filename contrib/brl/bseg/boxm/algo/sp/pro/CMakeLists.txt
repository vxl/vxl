# boxm's Process Library

# bprb depends on expat and bvxm_prodepends on bprb primarily
include( ${BRL_MODULE_PATH}/FindEXPAT.cmake )
if(EXPAT_FOUND)
  include_directories( ${BRL_INCLUDE_DIR}/bpro )
  include_directories( ${BRL_INCLUDE_DIR}/bpro/bprb )
  include_directories( ${BRL_INCLUDE_DIR}/bseg )
  include_directories( ${GEL_INCLUDE_DIR}/mrc/vpgl )
  include_directories( ${GEL_INCLUDE_DIR}/mrc )

  set(boxm_algo_sp_pro_sources
      boxm_algo_sp_processes.h
      boxm_algo_sp_register.h   boxm_algo_sp_register.cxx
     )
  aux_source_directory(Templates boxm_algo_sp_pro_sources)
  aux_source_directory(processes boxm_algo_sp_pro_sources)

  vxl_add_library(LIBRARY_NAME boxm_algo_sp_pro LIBRARY_SOURCES ${boxm_algo_sp_pro_sources} )
  target_link_libraries(boxm_algo_sp_pro boxm boxm_algo_sp boxm_util bprb brdb brip sdet vsol vil_pro ${VXL_LIB_PREFIX}vil_io ${VXL_LIB_PREFIX}vil)

  if( BUILD_TESTING )
    add_subdirectory(tests)
  endif()

endif()
