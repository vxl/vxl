# contrib/brl/bseg/bvxm_batch/CMakeLists.txt
INCLUDE( ${VXL_CMAKE_DIR}/NewCMake/FindPython.cmake )
IF( PYTHON_FOUND )
  INCLUDE_DIRECTORIES( ${PYTHON_INCLUDE_PATH} )
  INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR} )
  INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bpro )
  INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bpro/core )

  INCLUDE_DIRECTORIES( ${VXLCORE_INCLUDE_DIR} )
  INCLUDE_DIRECTORIES( ${GEL_INCLUDE_DIR}/mrc )
  INCLUDE_DIRECTORIES( ${GEL_INCLUDE_DIR}/mrc/vpgl )
  INCLUDE_DIRECTORIES( ${MUL_INCLUDE_DIR} )
  INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bseg/bvxm )
  
   SET(bvxm_batch_sources
       batch_bvxm.h batch_bvxm.cxx
       reg_bvxm.cxx
   )
 

  ADD_LIBRARY(bvxm_batch SHARED ${bvxm_batch_sources})

  #library has to have different name depending on debug or release version.

  SET_TARGET_PROPERTIES(bvxm_batch PROPERTIES OUTPUT_NAME bvxm_batch DEBUG_POSTFIX _d SUFFIX .pyd)

  TARGET_LINK_LIBRARIES(bvxm_batch bprb brdb vbl bvxm bvxm_pro bvxm_io vpgl_io vpgl_bpro vil_io vil_bpro)
  
  IF (PYTHON_DEBUG_LIBRARIES)
    TARGET_LINK_LIBRARIES( bvxm_batch debug ${PYTHON_DEBUG_LIBRARY})
  ENDIF(PYTHON_DEBUG_LIBRARIES )

  IF(PYTHON_LIBRARIES)
    TARGET_LINK_LIBRARIES( bvxm_batch optimized ${PYTHON_LIBRARY})
  ENDIF(PYTHON_LIBRARIES)
ENDIF( PYTHON_FOUND )
