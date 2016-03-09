# This is contrib/brl/bseg/bvpl/pro/CMakeLists.txt
# bvpl's Process Library

# bprb depends on expat and bvpl_pro depends on bprb primarily
include( ${BRL_MODULE_PATH}/FindEXPAT.cmake )
if(EXPAT_FOUND)
  include_directories( ${BRL_INCLUDE_DIR}/bpro )
  include_directories( ${BRL_INCLUDE_DIR}/bpro/bprb )
  include_directories( ${GEL_INCLUDE_DIR}/mrc )
  include_directories( ${MUL_INCLUDE_DIR} )
  set(bvpl_pro_sources
      bvpl_processes.h
      bvpl_register.h    bvpl_register.cxx
     )

  aux_source_directory(Templates bvpl_pro_sources)
  aux_source_directory(processes bvpl_pro_sources)
  aux_source_directory(processes/util bvpl_pro_sources)

  vxl_add_library(LIBRARY_NAME bvpl_pro LIBRARY_SOURCES ${bvpl_pro_sources})

  target_link_libraries(bvpl_pro bprb brdb bvpl bvpl_util bvpl_util_io bvpl_kernels_pro bvrml bwm_video ${VXL_LIB_PREFIX}vil_algo ${VXL_LIB_PREFIX}vil ${VXL_LIB_PREFIX}vgl ${VXL_LIB_PREFIX}vnl ${VXL_LIB_PREFIX}vul ${VXL_LIB_PREFIX}vbl_io)

  if( BUILD_TESTING )
    add_subdirectory(tests)
  endif()

endif()
