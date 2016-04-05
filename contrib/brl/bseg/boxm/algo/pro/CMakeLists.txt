# boxm's Process Library

# bprb depends on expat and bvxm_pro depends on bprb primarily
include( ${BRL_MODULE_PATH}/FindEXPAT.cmake )
if(EXPAT_FOUND)
  include_directories( ${BRL_INCLUDE_DIR}/bpro )
  include_directories( ${BRL_INCLUDE_DIR}/bpro/bprb )
  include_directories( ${BRL_INCLUDE_DIR}/bseg )
  include_directories( ${GEL_INCLUDE_DIR}/mrc/vpgl )
  include_directories( ${GEL_INCLUDE_DIR}/mrc )
  include_directories( ${VTHREEP_INCLUDE_DIR}/rply )

  set(boxm_algo_pro_sources
      boxm_algo_processes.h
      boxm_algo_register.h   boxm_algo_register.cxx
  )

  #aux_source_directory(Templates boxm_algo_pro_sources)
  aux_source_directory(processes boxm_algo_pro_sources)

  vxl_add_library(LIBRARY_NAME boxm_algo_pro LIBRARY_SOURCES ${boxm_algo_pro_sources} )

  target_link_libraries(boxm_algo_pro boxm_algo boxm boxm_sample boxm_util boct bprb brdb bsta_pro vil_pro bsta vpgl_pro ${VXL_LIB_PREFIX}vpgl ${VXL_LIB_PREFIX}vpgl_algo ${VXL_LIB_PREFIX}vil vsol ${VXL_LIB_PREFIX}vgl brip ${VXL_LIB_PREFIX}vcl rply)
  # Need bsta_pro only for the instantiation of brdb_value_t<vbl_smart_ptr<bsta_histogram_base> >
  # Need vpgl_pro only for the instantiation of brdb_value_t<vbl_smart_ptr<vpgl_camera<double> > >
  # Need vil_pro  only for the instantiation of brdb_value_t<vil_smart_ptr<vil_image_view_base> >

  if( BUILD_TESTING )
    add_subdirectory(tests)
  endif()

endif()
