# bprb depends on expat and boxm_ocl_pro depends on bprb primarily
include( ${BRL_MODULE_PATH}/FindEXPAT.cmake )
if(EXPAT_FOUND)
  include_directories( ${BRL_INCLUDE_DIR}/bpro )
  include_directories( ${BRL_INCLUDE_DIR}/bpro/bprb )
  include_directories( ${BRL_INCLUDE_DIR}/bseg )
  include_directories( ${GEL_INCLUDE_DIR}/mrc/vpgl )
  include_directories( ${GEL_INCLUDE_DIR}/mrc )

  set(bocl_pro_sources
      bocl_processes.h
      bocl_register.h   bocl_register.cxx
  )

  aux_source_directory(processes bocl_pro_sources)
  aux_source_directory(Templates bocl_pro_sources)

  vxl_add_library(LIBRARY_NAME bocl_pro LIBRARY_SOURCES ${bocl_pro_sources} )

  target_link_libraries(bocl_pro bocl bprb brdb )

  if( BUILD_TESTING )
    add_subdirectory(tests)
  endif()

endif()
