# boxm's Process Library

# bprb depends on expat and bvxm_pro depends on bprb primarily
INCLUDE( ${BRL_MODULE_PATH}/FindEXPAT.cmake )
IF(EXPAT_FOUND)
  INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bpro )
  INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bpro/bprb )
  INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bseg )
  INCLUDE_DIRECTORIES( ${GEL_INCLUDE_DIR}/mrc/vpgl )
  INCLUDE_DIRECTORIES( ${GEL_INCLUDE_DIR}/mrc )
  INCLUDE_DIRECTORIES( ${VTHREEP_INCLUDE_DIR}/rply )

  SET(boxm_algo_pro_sources
      boxm_algo_processes.h
      boxm_algo_register.h   boxm_algo_register.cxx
  )

  #AUX_SOURCE_DIRECTORY(Templates boxm_algo_pro_sources)
  AUX_SOURCE_DIRECTORY(processes boxm_algo_pro_sources)

  ADD_LIBRARY(boxm_algo_pro ${boxm_algo_pro_sources} )

  TARGET_LINK_LIBRARIES(boxm_algo_pro boxm_algo boxm boxm_sample boxm_util boct bprb brdb bsta_pro vil_pro bsta vpgl_pro vpgl vpgl_algo vil vsol vgl brip vcl rply)
  # Need bsta_pro only for the instantiation of brdb_value_t<vbl_smart_ptr<bsta_histogram_base> >
  # Need vpgl_pro only for the instantiation of brdb_value_t<vbl_smart_ptr<vpgl_camera<double> > >
  # Need vil_pro  only for the instantiation of brdb_value_t<vil_smart_ptr<vil_image_view_base> >

  IF( BUILD_TESTING )
    SUBDIRS(tests)
  ENDIF( BUILD_TESTING )

ENDIF(EXPAT_FOUND)
