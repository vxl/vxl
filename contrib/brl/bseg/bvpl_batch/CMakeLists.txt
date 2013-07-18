# contrib/brl/bseg/bvpl_batch/CMakeLists.txt
INCLUDE( ${VXL_CMAKE_DIR}/NewCMake/FindPython.cmake )
IF( PYTHON_FOUND )
  INCLUDE( ${BRL_MODULE_PATH}/FindEXPAT.cmake )
  IF( EXPAT_FOUND )
    INCLUDE_DIRECTORIES( ${PYTHON_INCLUDE_DIRS} )
    INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR} )
    INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bpro )
    INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bpro/core )

    INCLUDE_DIRECTORIES( ${VXLCORE_INCLUDE_DIR} )
    INCLUDE_DIRECTORIES( ${GEL_INCLUDE_DIR}/mrc )
    #INCLUDE_DIRECTORIES( ${GEL_INCLUDE_DIR}/mrc/vpgl )
    INCLUDE_DIRECTORIES( ${MUL_INCLUDE_DIR} )
    INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bseg/bvpl )
    INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bseg/brec )

     SET(bvpl_batch_sources
         reg_bvpl.h   reg_bvpl.cxx
        )
    IF (NOT BUILD_POSITION_DEPENDENT_CODE)
      ADD_LIBRARY(bvpl_batch SHARED ${bvpl_batch_sources})

      #library has to have different name depending on debug or release version.
      IF(WIN32)
        SET_TARGET_PROPERTIES(bvpl_batch PROPERTIES OUTPUT_NAME bvpl_batch DEBUG_POSTFIX _d SUFFIX .pyd)
      ENDIF(WIN32)
      SET_TARGET_PROPERTIES(bvpl_batch PROPERTIES PREFIX "")

      TARGET_LINK_LIBRARIES(bvpl_batch bprb brdb bpro_batch vbl bvpl bvpl_pro bvpl_kernels_pro bvxm_grid_pro bvxm_grid_io bvxm_algo_pro)

      IF (PYTHON_DEBUG_LIBRARIES)
        TARGET_LINK_LIBRARIES( bvpl_batch debug ${PYTHON_DEBUG_LIBRARY})
      ENDIF(PYTHON_DEBUG_LIBRARIES )

      IF(PYTHON_LIBRARIES)
        TARGET_LINK_LIBRARIES( bvpl_batch optimized ${PYTHON_LIBRARY})
      ENDIF(PYTHON_LIBRARIES)

      IF( BUILD_TESTING )
        SUBDIRS(tests)
      ENDIF( BUILD_TESTING )

    ENDIF(NOT BUILD_POSITION_DEPENDENT_CODE)
  ENDIF( EXPAT_FOUND )
ENDIF( PYTHON_FOUND )

INCLUDE( ${VXL_CMAKE_DIR}/NewCMake/FindDirectShow.cmake )
IF(DIRECTSHOW_FOUND)
  ADD_DEFINITIONS(-DHAS_DIRECTSHOW)
ENDIF(DIRECTSHOW_FOUND)
