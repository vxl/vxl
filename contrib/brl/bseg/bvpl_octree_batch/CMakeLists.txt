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
    IF (NOT BUILD_POSITION_DEPENDENT_CODE)
      ADD_LIBRARY(bvpl_octree_batch SHARED ${bvpl_octree_batch_sources})

      #In windows, library has to have different name depending on debug or release version.
      IF(WIN32)
        SET_TARGET_PROPERTIES(bvpl_octree_batch PROPERTIES OUTPUT_NAME bvpl_octree_batch DEBUG_POSTFIX _d SUFFIX .pyd)
      ENDIF(WIN32)
      IF(APPLE) #This is because python only looks for .so
        SET_TARGET_PROPERTIES(bvpl_octree_batch PROPERTIES OUTPUT_NAME bvpl_octree_batch SUFFIX .so)
      ENDIF(APPLE)
      SET_TARGET_PROPERTIES(bvpl_octree_batch PROPERTIES PREFIX "")

      TARGET_LINK_LIBRARIES(bvpl_octree_batch bvpl_octree_pro bvpl_kernels_pro bpro_batch boxm_pro boxm_algo_pro vpgl_io vpgl_pro vil_io vil_pro)

      IF (PYTHON_DEBUG_LIBRARIES)
        TARGET_LINK_LIBRARIES( bvpl_octree_batch debug ${PYTHON_DEBUG_LIBRARY})
      ENDIF(PYTHON_DEBUG_LIBRARIES )

      IF(PYTHON_LIBRARIES)
        TARGET_LINK_LIBRARIES( bvpl_octree_batch optimized ${PYTHON_LIBRARY})
      ENDIF(PYTHON_LIBRARIES)

      IF( BUILD_TESTING )
        SUBDIRS(tests)
      ENDIF( BUILD_TESTING )
    ENDIF (NOT BUILD_POSITION_DEPENDENT_CODE)
  ENDIF( EXPAT_FOUND )
ENDIF( PYTHON_FOUND )
