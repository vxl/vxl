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
  endif(TIFF_FOUND)
  include( ${VXL_CMAKE_DIR}/FindGEOTIFF.cmake)
  if(GEOTIFF_FOUND)
    include_directories(${GEOTIFF_INCLUDE_DIR})
    set(HAS_GEOTIFF 1 )
  endif(GEOTIFF_FOUND)

  add_library(boxm2_pro ${boxm2_pro_sources})

  #install the .h .txx and libs
  install_targets(/lib boxm2_pro)
  INSTALL_NOBASE_HEADER_FILES(/include/vxl/contrib/brl/bseg/boxm2/pro ${boxm2_pro_sources})

  target_link_libraries(boxm2_pro boxm2 boxm2_io boxm2_util boxm2_cpp_algo bbas_pro bvgl sdet bprb brdb brip imesh vsol bsta bsta_pro vpgl_pro vnl vil_algo vil_io vil vgl vbl_io vul bvrml bmsh3d_algo rply)

  if( BUILD_TESTING )
    add_subdirectory(tests)
  endif( BUILD_TESTING )

endif(EXPAT_FOUND)
