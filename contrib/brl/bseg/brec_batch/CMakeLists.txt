# contrib/brl/bseg/brec_batch/CMakeLists.txt
INCLUDE( ${VXL_CMAKE_DIR}/NewCMake/FindPython.cmake )
IF( PYTHON_FOUND )
  INCLUDE( ${BRL_MODULE_PATH}/FindEXPAT.cmake )
  IF( EXPAT_FOUND )
    INCLUDE_DIRECTORIES( ${PYTHON_INCLUDE_DIRS} )
    INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR} )
    INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bpro )
    INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bpro/core )

    INCLUDE_DIRECTORIES( ${VXLCORE_INCLUDE_DIR} )
    INCLUDE_DIRECTORIES( ${MUL_INCLUDE_DIR} )
    INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bseg/brec )
    INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bseg/bvxm )

    SET(brec_batch_sources
        reg_brec.cxx          reg_brec.h
       )
    IF(NOT BUILD_POSITION_DEPENDENT_CODE)
      ADD_LIBRARY(brec_batch SHARED ${brec_batch_sources})

      IF(WIN32)
        #library has to have different name depending on debug or release version.
        SET_TARGET_PROPERTIES(brec_batch PROPERTIES OUTPUT_NAME brec_batch DEBUG_POSTFIX _d SUFFIX .pyd)
      ENDIF(WIN32)
      SET_TARGET_PROPERTIES(brec_batch PROPERTIES PREFIX "")

      TARGET_LINK_LIBRARIES(brec_batch bprb brdb bpro_batch bmdl_pro bvxm_pro bvxm_io bvxm_grid_io brec_pro bbgm_pro bvgl_pro brip_pro bsvg_pro vbl vpgl_io vidl_pro vpgl_pro vil_io vil_pro)

      IF (PYTHON_DEBUG_LIBRARIES)
        TARGET_LINK_LIBRARIES( brec_batch debug ${PYTHON_DEBUG_LIBRARY})
      ENDIF(PYTHON_DEBUG_LIBRARIES )

      IF(PYTHON_LIBRARIES)
        TARGET_LINK_LIBRARIES( brec_batch optimized ${PYTHON_LIBRARY})
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
