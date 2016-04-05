# boxm2's Process Library

# bprb depends on expat and boxm2_pro depends on bprb primarily
include( ${BRL_MODULE_PATH}/FindEXPAT.cmake )
if(EXPAT_FOUND)
  include_directories( ${BRL_INCLUDE_DIR}/bpro/core )
  include_directories( ${BRL_INCLUDE_DIR}/bpro )
  include_directories( ${BRL_INCLUDE_DIR}/bpro/bprb )
  include_directories( ${BRL_INCLUDE_DIR}/bseg )
  include_directories( ${BRL_INCLUDE_DIR}/bbas )
  include_directories( ${GEL_INCLUDE_DIR}/mrc/vpgl )
  include_directories( ${GEL_INCLUDE_DIR}/mrc )
  include_directories( ${VTHREEP_INCLUDE_DIR}/rply )
if(OPENCL_FOUND)
  include_directories( ${OPENCL_INCLUDE_PATH} )
  set( BOXM2_VOLM_PRO_LIBS boxm2_ocl)
endif()

  set(boxm2_volm_pro_sources
      boxm2_volm_processes.h
      boxm2_volm_register.h   boxm2_volm_register.cxx
     )

  aux_source_directory(Templates boxm2_volm_pro_sources)
  if(OPENCL_FOUND)
    aux_source_directory(processes boxm2_volm_pro_sources)
    add_definitions(-DHAVE_OPENCL)
  endif()

  vxl_add_library(LIBRARY_NAME boxm2_volm_pro LIBRARY_SOURCES ${boxm2_volm_pro_sources})

  #install the .h .hxx and libs

  target_link_libraries(boxm2_volm_pro boxm2_volm boxm2_volm_io boxm2_io boxm2_util boxm2_cpp_algo bbas_pro bvgl sdet bprb brdb brip vsol bsta bsta_pro vpgl_pro ${VXL_LIB_PREFIX}vnl ${VXL_LIB_PREFIX}vil_algo ${VXL_LIB_PREFIX}vil_io ${VXL_LIB_PREFIX}vil ${VXL_LIB_PREFIX}vgl ${VXL_LIB_PREFIX}vbl_io ${VXL_LIB_PREFIX}vul bvrml ${BOXM2_VOLM_PRO_LIBS})

endif()

#tests
if( BUILD_TESTING )
  add_subdirectory(tests)
endif()
