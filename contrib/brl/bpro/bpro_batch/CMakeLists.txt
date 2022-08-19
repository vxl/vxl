# contrib/brl/bseg/bpro_batch/CMakeLists.txt
project(bpro_batch C)

if( Python3_Development_FOUND )
  include_directories( ${Python3_INCLUDE_DIRS} )
  include_directories( ${BRL_INCLUDE_DIR}/bpro )
  include_directories( ${BRL_INCLUDE_DIR}/bpro/core )

  set(bpro_batch_sources
    bpro_batch.h              bpro_batch.cxx
  )

  vxl_add_library(LIBRARY_NAME bpro_batch LIBRARY_SOURCES  ${bpro_batch_sources})
  target_link_libraries(bpro_batch bprb brdb bbas_pro ${Python3_LIBRARIES})

  if( BUILD_TESTING )
    add_subdirectory(tests)
  endif()

endif()
