# contrib/brl/bseg/bpro_batch/CMakeLists.txt
project(bpro_batch C)
include( ${VXL_CMAKE_DIR}/NewCMake/FindPython.cmake )
if( PYTHON_FOUND )
  include_directories( ${PYTHON_INCLUDE_DIRS} )
  include_directories( ${BRL_INCLUDE_DIR}/bpro )
  include_directories( ${BRL_INCLUDE_DIR}/bpro/core )

  set(bpro_batch_sources
    bpro_batch.h              bpro_batch.cxx
  )

  vxl_add_library(LIBRARY_NAME bpro_batch LIBRARY_SOURCES  ${bpro_batch_sources})
  target_link_libraries(bpro_batch bprb brdb bbas_pro ${PYTHON_LIBRARY})

  if( BUILD_TESTING )
    add_subdirectory(tests)
  endif()

endif()
