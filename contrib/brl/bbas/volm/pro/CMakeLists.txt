# This is contrib/brl/bbas/volm/pro/CMakeLists.txt
# volm Process Library
if(VXL_BUILD_RPL)
include_directories( ${BRL_INCLUDE_DIR}/bbas )
include_directories( ${BRL_INCLUDE_DIR}/bpro/core )
include_directories( ${MUL_INCLUDE_DIR} )
include_directories( ${VTHREEP_INCLUDE_DIR}/rply)

  # The N-dimensional bin structure implementations use rpl/rsdl

  include_directories(${RPL_INCLUDE_DIR})


  set(volm_pro_sources
      volm_processes.h
      volm_register.h    volm_register.cxx
  )

  aux_source_directory(processes volm_pro_sources)

  aux_source_directory(Templates volm_pro_sources)

  vxl_add_library(LIBRARY_NAME volm_pro LIBRARY_SOURCES ${volm_pro_sources})
  target_link_libraries( volm_pro bstm_cpp_pro boxm2_cpp volm bprb brip  ${VXL_LIB_PREFIX}vul bmsh3d_algo rply bsol ${VXL_LIB_PREFIX}vgl ${VXL_LIB_PREFIX}vil ${VXL_LIB_PREFIX}vul bmsh3d bvgl bvgl_algo vpgl_pro bbas_pro)
  # Need vpgl_pro only for the instantiation of brdb_value_t<vbl_smart_ptr<vpgl_camera<double> > >

  if(VXL_BUILD_RPL)
    target_link_libraries(volm_pro rsdl)
  endif()

  if( BUILD_TESTING )
    add_subdirectory(tests)
  endif()

endif()
