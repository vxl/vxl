# contrib/brl/bseg/boxm_batch/CMakeLists.txt
INCLUDE( ${VXL_CMAKE_DIR}/NewCMake/FindPython.cmake )
INCLUDE( ${VXL_CMAKE_DIR}/NewCMake/FindOpenCL.cmake )

IF( PYTHON_FOUND )
    MESSAGE("${PYTHON_INCLUDE_DIRS}")
    MESSAGE("${Python_FRAMEWORKS}")
  INCLUDE( ${BRL_MODULE_PATH}/FindEXPAT.cmake )
  IF( EXPAT_FOUND )
    INCLUDE_DIRECTORIES( ${PYTHON_INCLUDE_DIRS} )
    INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR} )
    INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bpro )
    INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bpro/core )

    INCLUDE_DIRECTORIES( ${VXLCORE_INCLUDE_DIR} )
    INCLUDE_DIRECTORIES( ${GEL_INCLUDE_DIR}/mrc )
    INCLUDE_DIRECTORIES( ${GEL_INCLUDE_DIR}/mrc/vpgl )
    INCLUDE_DIRECTORIES( ${MUL_INCLUDE_DIR} )
    INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bseg/boxm )
    INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bseg/boxm/algo )
    INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bseg/brec )
    INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bbas/bsta )

     SET(boxm_batch_sources
       reg_boxm.h   reg_boxm.cxx
     )

    ADD_LIBRARY(boxm_batch SHARED ${boxm_batch_sources})

    #library has to have different name depending on debug or release version.
    IF(WIN32)
      SET_TARGET_PROPERTIES(boxm_batch PROPERTIES OUTPUT_NAME boxm_batch DEBUG_POSTFIX _d SUFFIX .pyd)
    ENDIF(WIN32)
    IF(APPLE)
      SET_TARGET_PROPERTIES(boxm_batch PROPERTIES OUTPUT_NAME boxm_batch SUFFIX .so)
    ENDIF(APPLE)
    SET_TARGET_PROPERTIES(boxm_batch PROPERTIES PREFIX "")
    IF(NOT WIN32)
      SET_TARGET_PROPERTIES(boxm_batch PROPERTIES COMPILE_FLAGS "-fPIC")
    ENDIF(NOT WIN32)

    TARGET_LINK_LIBRARIES(boxm_batch bpro_batch bbas_pro boxm_pro boxm_sample_algo boxm_algo_rt_pro boxm_algo_sp_pro boxm_algo_pro vpgl_io vpgl_pro vil_io vil_pro sdet_pro bvpl_octree_pro bsta_pro)

    IF(OPENCL_FOUND)
      ADD_DEFINITIONS(-DHAS_OPENCL=1)
      TARGET_LINK_LIBRARIES(boxm_batch boxm_ocl_pro)
    ENDIF(OPENCL_FOUND)

    IF (PYTHON_DEBUG_LIBRARIES)
      TARGET_LINK_LIBRARIES( boxm_batch debug ${PYTHON_DEBUG_LIBRARY})
    ENDIF(PYTHON_DEBUG_LIBRARIES )

    IF(PYTHON_LIBRARIES)
      TARGET_LINK_LIBRARIES( boxm_batch optimized ${PYTHON_LIBRARY})
    ENDIF(PYTHON_LIBRARIES)

    IF( BUILD_TESTING )
      SUBDIRS(tests)
    ENDIF( BUILD_TESTING )

  ENDIF( EXPAT_FOUND )
ENDIF( PYTHON_FOUND )

INCLUDE( ${MODULE_PATH}/NewCMake/FindDirectShow.cmake )
IF(DIRECTSHOW_FOUND)
  ADD_DEFINITIONS(-DHAS_DIRECTSHOW)
ENDIF(DIRECTSHOW_FOUND)
