# contrib/brl/bseg/bvxm_batch/CMakeLists.txt
if(HAS_GEOTIFF)
include( ${VXL_CMAKE_DIR}/NewCMake/FindPython.cmake )
if( PYTHON_FOUND )
  include( ${BRL_MODULE_PATH}/FindEXPAT.cmake )
  if( EXPAT_FOUND )
    include_directories( ${PYTHON_INCLUDE_DIRS} )
    include_directories( ${BRL_INCLUDE_DIR} )
    include_directories( ${BRL_INCLUDE_DIR}/bpro )
    include_directories( ${BRL_INCLUDE_DIR}/bpro/core )

    include_directories( ${GEL_INCLUDE_DIR}/mrc )
    include_directories( ${GEL_INCLUDE_DIR}/mrc/vpgl )
    include_directories( ${MUL_INCLUDE_DIR} )
    include_directories( ${BRL_INCLUDE_DIR}/bseg/bvxm )
    include_directories( ${BRL_INCLUDE_DIR}/bseg/brec )

     set(bvxm_batch_sources
         reg_bvxm.h   reg_bvxm.cxx
        )
    if(NOT VXL_BUILD_POSITION_DEPENDENT_CODE)
      vxl_add_library(LIBRARY_NAME bvxm_batch LIBRARY_SOURCES SHARED ${bvxm_batch_sources})

      #library has to have different name depending on debug or release version.
      if(WIN32)
        if(NOT CYGWIN)
          set_target_properties(bvxm_batch PROPERTIES OUTPUT_NAME bvxm_batch DEBUG_POSTFIX _d SUFFIX .pyd)
        endif()
      endif()
      set_target_properties(bvxm_batch PROPERTIES PREFIX "")

      target_link_libraries(bvxm_batch bprb brdb bpro_batch ${VXL_LIB_PREFIX}vbl bvxm bvxm_pro bvxm_io bvxm_grid_io bvxm_grid_pro ${VXL_LIB_PREFIX}vpgl_io vpgl_pro ${VXL_LIB_PREFIX}vil_io vil_pro sdet_pro brad_pro brad_io bvrml_pro volm_pro brip_pro bapl_pro bvgl_pro bsgm_pro vcon_pro ihog_pro)

      if(PYTHON_DEBUG_LIBRARIES)
        target_link_libraries( bvxm_batch debug ${PYTHON_DEBUG_LIBRARY})
      endif()

      if(PYTHON_LIBRARIES)
        target_link_libraries( bvxm_batch optimized ${PYTHON_LIBRARY})
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
else()
  message(STATUS "Skipping contrib/brl/bseg/bvxm_batch: requires geotiff")
endif()
