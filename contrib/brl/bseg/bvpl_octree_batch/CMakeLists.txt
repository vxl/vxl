# contrib/brl/bseg/bvpl_octree_batch/CMakeLists.txt
INCLUDE( ${VXL_CMAKE_DIR}/NewCMake/FindPython.cmake )
IF( PYTHON_FOUND )
  INCLUDE( ${BRL_MODULE_PATH}/FindEXPAT.cmake )
  IF( EXPAT_FOUND )
    INCLUDE_DIRECTORIES( ${PYTHON_INCLUDE_DIRS} )
    INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR} )
    INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bpro )
    INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bpro/core )
    INCLUDE_DIRECTORIES( ${VXLCORE_INCLUDE_DIR} )
    INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bseg/bvpl/bvpl_octree)
    INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bseg/brec )

     SET(bvpl_octree_batch_sources
       reg_bvpl_octree.h   reg_bvpl_octree.cxx
     )

    ADD_LIBRARY(bvpl_octree_batch MODULE ${bvpl_octree_batch_sources})

    #In windows, library has to have different name depending on debug or release version.
    IF(WIN32)
      SET_TARGET_PROPERTIES(bvpl_octree_batch PROPERTIES OUTPUT_NAME bvpl_octree_batch DEBUG_POSTFIX _d SUFFIX .pyd)
    ENDIF(WIN32)
    SET_TARGET_PROPERTIES(bvpl_octree_batch PROPERTIES PREFIX "")
    IF(NOT WIN32)
      SET_TARGET_PROPERTIES(boxm_batch PROPERTIES COMPILE_FLAGS "-fPIC")
    ENDIF(NOT WIN32)

    TARGET_LINK_LIBRARIES(bvpl_octree_batch bprb brdb bpro_batch vbl bvpl_octree_pro bvpl_pro bvpl_io boxm_pro boxm_algo_pro bvxm_grid_pro bvxm_grid_io vpgl_io vpgl_pro vil_io vil_pro)

    IF (PYTHON_DEBUG_LIBRARIES)
      TARGET_LINK_LIBRARIES( bvpl_octree_batch debug ${PYTHON_DEBUG_LIBRARY})
    ENDIF(PYTHON_DEBUG_LIBRARIES )

    IF(PYTHON_LIBRARIES)
      TARGET_LINK_LIBRARIES( bvpl_octree_batch optimized ${PYTHON_LIBRARY})
    ENDIF(PYTHON_LIBRARIES)
  ENDIF( EXPAT_FOUND )
ENDIF( PYTHON_FOUND )
