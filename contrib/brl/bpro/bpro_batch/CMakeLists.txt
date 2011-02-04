# contrib/brl/bseg/bpro_batch/CMakeLists.txt
PROJECT(bpro_batch C)
INCLUDE( ${VXL_CMAKE_DIR}/NewCMake/FindPython.cmake )
IF( PYTHON_FOUND )
  INCLUDE_DIRECTORIES( ${PYTHON_INCLUDE_DIRS} )
  INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bpro )
  INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bpro/core )

  INCLUDE_DIRECTORIES( ${VXLCORE_INCLUDE_DIR} )
  SET(bpro_batch_sources
    bpro_batch.h              bpro_batch.cxx
  )

  ADD_LIBRARY(bpro_batch  ${bpro_batch_sources})
  TARGET_LINK_LIBRARIES(bpro_batch bprb brdb bbas_pro ${PYTHON_LIBRARY})

  IF( BUILD_TESTING )
    SUBDIRS(tests)
  ENDIF( BUILD_TESTING )

ENDIF( PYTHON_FOUND )
