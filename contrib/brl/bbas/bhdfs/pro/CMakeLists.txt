# bhdfs's Process Library

# bprb depends on expat and bhdfs_pro depends on bprb primarily
include( ${BRL_MODULE_PATH}/FindEXPAT.cmake )
if(EXPAT_FOUND)
  include_directories( ${BRL_INCLUDE_DIR}/bpro )
  include_directories( ${BRL_INCLUDE_DIR}/bpro/bprb )
  include_directories( ${BRL_INCLUDE_DIR}/bbas )
  include_directories( ${EXPAT_INCLUDE_DIR} )
  include_directories( ${BRL_INCLUDE_DIR}/b3p/expatpp )

  set(bhdfs_pro_sources
      bhdfs_processes.h
      bhdfs_register.h   bhdfs_register.cxx
     )

  aux_source_directory(Templates bhdfs_pro_sources)
  aux_source_directory(processes bhdfs_pro_sources)

  vxl_add_library(LIBRARY_NAME bhdfs_pro LIBRARY_SOURCES ${bhdfs_pro_sources})

  target_link_libraries(bhdfs_pro bhdfs bprb ${VXL_LIB_PREFIX}vil_algo ${VXL_LIB_PREFIX}vil_io ${VXL_LIB_PREFIX}vil ${VXL_LIB_PREFIX}vpgl boxm2)

  if(BUILD_TESTING)
    add_subdirectory(tests)
  endif()

endif()
