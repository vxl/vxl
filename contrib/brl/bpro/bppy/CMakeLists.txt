# brl/bpro/bppy/CMakeLists.txt
#update vxl/config/cmake/Modules folder to get the following

INCLUDE( ${VXL_CMAKE_DIR}/NewCMake/FindPython.cmake )
IF( PYTHON_FOUND )
  INCLUDE_DIRECTORIES( ${PYTHON_INCLUDE_PATH} )
  INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR} )
  INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bpro )

  SET(batch_sources
    batch.h  batch.cxx
    reg.cxx 
  )

  ADD_LIBRARY(batch SHARED ${batch_sources})
  SET_TARGET_PROPERTIES(batch PROPERTIES OUTPUT_NAME batch DEBUG_POSTFIX _d SUFFIX .pyd)
  TARGET_LINK_LIBRARIES(batch bprb bsta_algo bsta bbgm brdb bbgm_pro vidl2_bpro vil_pro vbl)

IF (PYTHON_DEBUG_LIBRARY)
 TARGET_LINK_LIBRARIES( batch debug ${PYTHON_DEBUG_LIBRARY})
ENDIF(PYTHON_DEBUG_LIBRARY )

IF(PYTHON_LIBRARY)
 TARGET_LINK_LIBRARIES( batch optimized ${PYTHON_LIBRARY})
ENDIF(PYTHON_LIBRARY)

ENDIF( PYTHON_FOUND )
