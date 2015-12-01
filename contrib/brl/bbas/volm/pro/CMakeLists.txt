# This is contrib/brl/bbas/volm/pro/CMakeLists.txt
# volm Process Library
IF(BUILD_RPL)
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bbas )
INCLUDE_DIRECTORIES( ${VTHREEP_INCLUDE_DIR}/rply)

  # The N-dimensional bin structure implementations use rpl/rsdl

  INCLUDE_DIRECTORIES(${RPL_INCLUDE_DIR})


  SET(volm_pro_sources
      volm_processes.h
      volm_register.h    volm_register.cxx
  )

  AUX_SOURCE_DIRECTORY(processes volm_pro_sources)

  AUX_SOURCE_DIRECTORY(Templates volm_pro_sources)

  ADD_LIBRARY(volm_pro ${volm_pro_sources})
  TARGET_LINK_LIBRARIES( volm_pro bstm_cpp_pro boxm2_cpp volm bprb brip vul bmsh3d_algo rply bsol vgl vil vul bmsh3d bvgl bvgl_algo vpgl_pro)
  # Need vpgl_pro only for the instantiation of brdb_value_t<vbl_smart_ptr<vpgl_camera<double> > >

  IF(BUILD_RPL)
    TARGET_LINK_LIBRARIES(volm_pro rsdl)
  ENDIF(BUILD_RPL)

  IF( BUILD_TESTING )
    add_subdirectory(tests)
  ENDIF( BUILD_TESTING )

ENDIF(BUILD_RPL)
