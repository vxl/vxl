# boxm's Process Library

# bprb depends on expat and bvxm_prodepends on bprb primarily
include( ${BRL_MODULE_PATH}/FindEXPAT.cmake )
if(EXPAT_FOUND)
  include_directories( ${BRL_INCLUDE_DIR}/bpro )
  include_directories( ${BRL_INCLUDE_DIR}/bpro/bprb )
  include_directories( ${BRL_INCLUDE_DIR}/bseg )
  include_directories( ${GEL_INCLUDE_DIR}/mrc/vpgl )
  include_directories( ${GEL_INCLUDE_DIR}/mrc )

  set(boxm_pro_sources
      boxm_processes.h
      boxm_register.h   boxm_register.cxx
     )

  aux_source_directory(Templates boxm_pro_sources)
  aux_source_directory(processes boxm_pro_sources)

  add_library(boxm_pro ${boxm_pro_sources})

  target_link_libraries(boxm_pro boxm boxm_util boxm_sample_algo boxm_sample vpgl_pro imesh bprb brdb brip vsol bsta bsta_pro vnl_io vnl vil vgl vbl_io vul vbl)

  if( BUILD_TESTING )
    add_subdirectory(tests)
  endif( BUILD_TESTING )

endif(EXPAT_FOUND)
