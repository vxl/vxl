# contrib/brl/bseg/boxm_batch/CMakeLists.txt

include( ${VXL_CMAKE_DIR}/NewCMake/FindPython.cmake )
include( ${VXL_CMAKE_DIR}/NewCMake/FindOpenCL.cmake )

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
    include_directories( ${BRL_INCLUDE_DIR}/bseg/boxm )
    include_directories( ${BRL_INCLUDE_DIR}/bseg/boxm/algo )
    include_directories( ${BRL_INCLUDE_DIR}/bseg/brec )
    include_directories( ${BRL_INCLUDE_DIR}/bbas/bsta )

     set(boxm_batch_sources
         reg_boxm.h   reg_boxm.cxx
        )
    if(NOT VXL_BUILD_POSITION_DEPENDENT_CODE)
      vxl_add_library(LIBRARY_NAME boxm_batch LIBRARY_SOURCES SHARED ${boxm_batch_sources})

      #library has to have different name depending on debug or release version.
      if(WIN32)
        set_target_properties(boxm_batch PROPERTIES OUTPUT_NAME boxm_batch DEBUG_POSTFIX _d SUFFIX .pyd)
      endif()
      if(APPLE)
        set_target_properties(boxm_batch PROPERTIES OUTPUT_NAME boxm_batch SUFFIX .so)
      endif()
      set_target_properties(boxm_batch PROPERTIES PREFIX "")

      target_link_libraries(boxm_batch bpro_batch boxm_pro boxm_sample_algo boxm_algo_rt_pro boxm_algo_sp_pro boxm_algo_pro ${VXL_LIB_PREFIX}vpgl_io vpgl_pro ${VXL_LIB_PREFIX}vil_io vil_pro sdet_pro bsta_pro icam_pro bapl_pro )

      if(PYTHON_DEBUG_LIBRARIES)
        target_link_libraries( boxm_batch debug ${PYTHON_DEBUG_LIBRARY})
      endif()

      if(PYTHON_LIBRARIES)
        target_link_libraries( boxm_batch optimized ${PYTHON_LIBRARY})
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
