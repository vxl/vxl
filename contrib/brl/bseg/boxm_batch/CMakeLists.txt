# contrib/brl/bseg/boxm_batch/CMakeLists.txt
INCLUDE( ${BRL_MODULE_PATH}/find_open_cl.cmake ) # so we know whether to include gpu optimized processes
INCLUDE( ${VXL_CMAKE_DIR}/NewCMake/FindPython.cmake )
IF( PYTHON_FOUND )
  INCLUDE( ${BRL_MODULE_PATH}/FindEXPAT.cmake )
  IF( EXPAT_FOUND )
    INCLUDE_DIRECTORIES( ${PYTHON_INCLUDE_PATH} )
    INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR} )
    INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bpro )
    INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bpro/core )

    INCLUDE_DIRECTORIES( ${VXLCORE_INCLUDE_DIR} )
    INCLUDE_DIRECTORIES( ${GEL_INCLUDE_DIR}/mrc )
    INCLUDE_DIRECTORIES( ${GEL_INCLUDE_DIR}/mrc/vpgl )
    INCLUDE_DIRECTORIES( ${MUL_INCLUDE_DIR} )
    INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bseg/boxm )
    INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bseg/boxm/opt )
    INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bseg/brec )
  
     SET(boxm_batch_sources
       reg_boxm.h   reg_boxm.cxx
     )

    ADD_LIBRARY(boxm_batch MODULE ${boxm_batch_sources})

    #library has to have different name depending on debug or release version.
    IF(WIN32)
      SET_TARGET_PROPERTIES(boxm_batch PROPERTIES OUTPUT_NAME boxm_batch DEBUG_POSTFIX _d SUFFIX .pyd)
    ENDIF(WIN32)
    SET_TARGET_PROPERTIES(boxm_batch PROPERTIES PREFIX "")


    TARGET_LINK_LIBRARIES(boxm_batch bpro_batch boxm_pro boxm_opt_pro boxm_algo_pro bvpl_octree_pro vpgl_io vpgl_pro vil_io vil_pro sdet_pro)

    IF(OPENCL_FOUND)
      TARGET_LINK_LIBRARIES(boxm_batch boxm_opt_opencl_pro)
    ENDIF(OPENCL_FOUND)
  
    IF (PYTHON_DEBUG_LIBRARIES)
      TARGET_LINK_LIBRARIES( boxm_batch debug ${PYTHON_DEBUG_LIBRARY})
    ENDIF(PYTHON_DEBUG_LIBRARIES )

    IF(PYTHON_LIBRARIES)
      TARGET_LINK_LIBRARIES( boxm_batch optimized ${PYTHON_LIBRARY})
    ENDIF(PYTHON_LIBRARIES)
  ENDIF( EXPAT_FOUND ) 
ENDIF( PYTHON_FOUND )

INCLUDE( ${MODULE_PATH}/NewCMake/FindDirectShow.cmake )
IF(DIRECTSHOW_FOUND)
  ADD_DEFINITIONS(-DHAS_DIRECTSHOW)
ENDIF(DIRECTSHOW_FOUND)
