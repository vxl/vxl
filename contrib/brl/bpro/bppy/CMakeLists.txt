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

  # checking PIC compatibility not necessary if we are building shared libs anyway
  IF( NOT BUILD_SHARED_LIBS)
    IF( NOT ${VXL_PIC_COMPATIBLE})
      MESSAGE("Warning: linking a shared library to static VXL libraries will probably fail.\n"
              "Try adding -fPIC to CMAKE_CXX_FLAGS and CMAKE_C_FLAGS.\n"
              "This will compile the static libraries as Position-Independent Code.\n"
              "However, this option could make the static executables slightly less efficient.")
      # Reset the update configuration flag
      SET( VXL_UPDATE_CONFIGURATION "ON" CACHE BOOL "Re-run the configuration tests?" FORCE )
    ENDIF( NOT ${VXL_PIC_COMPATIBLE})
  ENDIF( NOT BUILD_SHARED_LIBS)
  
  ADD_LIBRARY(batch SHARED ${batch_sources})
  SET_TARGET_PROPERTIES(batch PROPERTIES OUTPUT_NAME batch DEBUG_POSTFIX _d SUFFIX .pyd)
  TARGET_LINK_LIBRARIES(batch bprb bsta_algo bsta bbgm brdb bbgm_pro vidl2_pro vil_pro vbl)

IF (PYTHON_DEBUG_LIBRARY)
 TARGET_LINK_LIBRARIES( batch debug ${PYTHON_DEBUG_LIBRARY})
ENDIF(PYTHON_DEBUG_LIBRARY )

IF(PYTHON_LIBRARY)
 TARGET_LINK_LIBRARIES( batch optimized ${PYTHON_LIBRARY})
ENDIF(PYTHON_LIBRARY)

ENDIF( PYTHON_FOUND )
