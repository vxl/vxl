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

  set(boxm2_pro_sources
      boxm2_processes.h
      boxm2_register.h   boxm2_register.cxx
     )

  aux_source_directory(Templates boxm2_pro_sources)
  aux_source_directory(processes boxm2_pro_sources)

  include(${VXL_CMAKE_DIR}/FindTIFF.cmake)
  if(TIFF_FOUND)
    include_directories(${TIFF_INCLUDE_DIR})
    set( HAS_TIFF 1 )
  endif()
  include( ${VXL_CMAKE_DIR}/FindGEOTIFF.cmake)
  if(GEOTIFF_FOUND)
    include_directories(${GEOTIFF_INCLUDE_DIR})
    set(HAS_GEOTIFF 1 )
  endif()

  vxl_add_library(LIBRARY_NAME boxm2_pro LIBRARY_SOURCES ${boxm2_pro_sources})

  #install the .h .hxx and libs

  target_link_libraries(boxm2_pro boxm2 boxm2_io boxm2_util boxm2_cpp_algo bbas_pro bvgl sdet bprb brdb brip imesh vsol bsta bsta_pro vpgl_pro ${VXL_LIB_PREFIX}vnl ${VXL_LIB_PREFIX}vil_algo ${VXL_LIB_PREFIX}vil_io ${VXL_LIB_PREFIX}vil ${VXL_LIB_PREFIX}vgl ${VXL_LIB_PREFIX}vbl_io ${VXL_LIB_PREFIX}vul bvrml bmsh3d_algo rply)

  if( BUILD_TESTING )
    add_subdirectory(tests)
  endif()

endif()
