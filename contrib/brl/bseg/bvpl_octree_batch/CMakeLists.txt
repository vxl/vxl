# contrib/brl/bseg/bvpl_octree_batch/CMakeLists.txt
include( ${VXL_CMAKE_DIR}/NewCMake/FindPython.cmake )
if( PYTHON_FOUND )
  include( ${BRL_MODULE_PATH}/FindEXPAT.cmake )
  if( EXPAT_FOUND )
    include_directories( ${PYTHON_INCLUDE_DIRS} )
    include_directories( ${BRL_INCLUDE_DIR} )
    include_directories( ${BRL_INCLUDE_DIR}/bpro )
    include_directories( ${BRL_INCLUDE_DIR}/bpro/core )
    include_directories( ${BRL_INCLUDE_DIR}/bseg/bvpl/bvpl_octree)
    include_directories( ${BRL_INCLUDE_DIR}/bseg/brec )

     set(bvpl_octree_batch_sources
       reg_bvpl_octree.h   reg_bvpl_octree.cxx
     )
    if(NOT VXL_BUILD_POSITION_DEPENDENT_CODE)
      vxl_add_library(LIBRARY_NAME bvpl_octree_batch LIBRARY_SOURCES SHARED ${bvpl_octree_batch_sources})

      #In windows, library has to have different name depending on debug or release version.
      if(WIN32)
        set_target_properties(bvpl_octree_batch PROPERTIES OUTPUT_NAME bvpl_octree_batch DEBUG_POSTFIX _d SUFFIX .pyd)
      endif()
      if(APPLE) #This is because python only looks for .so
        set_target_properties(bvpl_octree_batch PROPERTIES OUTPUT_NAME bvpl_octree_batch SUFFIX .so)
      endif()
      set_target_properties(bvpl_octree_batch PROPERTIES PREFIX "")

      target_link_libraries(bvpl_octree_batch bvpl_octree_pro bvpl_kernels_pro bpro_batch boxm_pro boxm_algo_pro ${VXL_LIB_PREFIX}vpgl_io vpgl_pro ${VXL_LIB_PREFIX}vil_io vil_pro)

      if(PYTHON_DEBUG_LIBRARIES)
        target_link_libraries( bvpl_octree_batch debug ${PYTHON_DEBUG_LIBRARY})
      endif()

      if(PYTHON_LIBRARIES)
        target_link_libraries( bvpl_octree_batch optimized ${PYTHON_LIBRARY})
      endif()

      if( BUILD_TESTING )
        add_subdirectory(tests)
      endif()
    endif()
  endif()
endif()
