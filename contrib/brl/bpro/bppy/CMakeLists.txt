# brl/bpro/bppy/CMakeLists.txt
#update vxl/config/cmake/Modules folder to get the following

INCLUDE( ${VXL_CMAKE_DIR}/NewCMake/FindPython.cmake )
IF( PYTHON_FOUND )
IF (BUILD_EXPAT)
  INCLUDE_DIRECTORIES( ${PYTHON_INCLUDE_PATH} )
  INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR} )
  INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bpro )

  SET(batch_sources
    batch.h  batch.cxx
    reg.cxx 
  )

  ADD_LIBRARY(batch SHARED ${batch_sources})
  SET_TARGET_PROPERTIES(batch PROPERTIES OUTPUT_NAME batch DEBUG_POSTFIX _d SUFFIX .pyd)
  TARGET_LINK_LIBRARIES(batch bprb bsta_algo bsta bbgm brdb bbgm_bpro vidl2_bpro vil_bpro vbl)

IF (PYTHON_DEBUG_LIBRARIES)
 TARGET_LINK_LIBRARIES( batch debug ${PYTHON_DEBUG_LIBRARIES})
ENDIF(PYTHON_DEBUG_LIBRARIES )

IF(PYTHON_LIBRARIES)
 TARGET_LINK_LIBRARIES( batch optimized ${PYTHON_LIBRARIES})
ENDIF(PYTHON_LIBRARIES)
ENDIF (BUILD_EXPAT)
ENDIF( PYTHON_FOUND )
