# This is contrib/brl/bseg/bvpl/kernels/pro/CMakeLists.txt
# BVPL kenel's related processes

# bprb depends on expat and bvxm_pro depends on bprb primarily
include( ${BRL_MODULE_PATH}/FindEXPAT.cmake )
if(EXPAT_FOUND)
  include_directories( ${BRL_INCLUDE_DIR}/bpro )
  include_directories( ${BRL_INCLUDE_DIR}/bpro/bprb )
  include_directories( ${GEL_INCLUDE_DIR}/mrc )
  include_directories( ${MUL_INCLUDE_DIR} )
  set(bvpl_kernels_pro_sources
      bvpl_kernels_processes.h
      bvpl_kernels_register.h    bvpl_kernels_register.cxx
     )

  aux_source_directory(Templates bvpl_kernels_pro_sources)
  aux_source_directory(processes bvpl_kernels_pro_sources)

  vxl_add_library(LIBRARY_NAME bvpl_kernels_pro LIBRARY_SOURCES ${bvpl_kernels_pro_sources})

  target_link_libraries(bvpl_kernels_pro bvpl_kernels bvpl_kernels_io bvpl_octree bvpl_octree_io bsta bprb brdb bvpl ${VXL_LIB_PREFIX}vnl ${VXL_LIB_PREFIX}vul ${VXL_LIB_PREFIX}vbl_io)

  if( BUILD_TESTING )
    add_subdirectory(tests)
  endif()

endif()
