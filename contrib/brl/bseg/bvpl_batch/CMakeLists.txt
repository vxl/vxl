# contrib/brl/bseg/bvpl_batch/CMakeLists.txt
include( ${VXL_CMAKE_DIR}/NewCMake/FindPython.cmake )
if( PYTHON_FOUND )
  include( ${BRL_MODULE_PATH}/FindEXPAT.cmake )
  if( EXPAT_FOUND )
    include_directories( ${PYTHON_INCLUDE_DIRS} )
    include_directories( ${BRL_INCLUDE_DIR} )
    include_directories( ${BRL_INCLUDE_DIR}/bpro )
    include_directories( ${BRL_INCLUDE_DIR}/bpro/core )

    include_directories( ${GEL_INCLUDE_DIR}/mrc )
    #include_directories( ${GEL_INCLUDE_DIR}/mrc/vpgl )
    include_directories( ${MUL_INCLUDE_DIR} )
    include_directories( ${BRL_INCLUDE_DIR}/bseg/bvpl )
    include_directories( ${BRL_INCLUDE_DIR}/bseg/brec )

     set(bvpl_batch_sources
         reg_bvpl.h   reg_bvpl.cxx
        )
    if(NOT VXL_BUILD_POSITION_DEPENDENT_CODE)
      vxl_add_library(LIBRARY_NAME bvpl_batch LIBRARY_SOURCES SHARED ${bvpl_batch_sources})

      #library has to have different name depending on debug or release version.
      if(WIN32)
        set_target_properties(bvpl_batch PROPERTIES OUTPUT_NAME bvpl_batch DEBUG_POSTFIX _d SUFFIX .pyd)
      endif()
      set_target_properties(bvpl_batch PROPERTIES PREFIX "")

      target_link_libraries(bvpl_batch bprb brdb bpro_batch ${VXL_LIB_PREFIX}vbl bvpl bvpl_pro bvpl_kernels_pro bvxm_grid_pro bvxm_grid_io bvxm_algo_pro)

      if(PYTHON_DEBUG_LIBRARIES)
        target_link_libraries( bvpl_batch debug ${PYTHON_DEBUG_LIBRARY})
      endif()

      if(PYTHON_LIBRARIES)
        target_link_libraries( bvpl_batch optimized ${PYTHON_LIBRARY})
      endif()

      if( BUILD_TESTING )
        add_subdirectory(tests)
      endif()

    endif()
  endif()
endif()

include( ${VXL_CMAKE_DIR}/NewCMake/FindDirectShow.cmake )
if(DIRECTSHOW_FOUND)
  add_definitions(-DHAS_DIRECTSHOW)
endif()
