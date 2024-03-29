# contrib/brl/bseg/brec_batch/CMakeLists.txt

if( Python3_Development_FOUND )
  include( ${BRL_MODULE_PATH}/FindEXPAT.cmake )
  if( EXPAT_FOUND )
    include_directories( ${Python3_INCLUDE_DIRS} )
    include_directories( ${BRL_INCLUDE_DIR} )
    include_directories( ${BRL_INCLUDE_DIR}/bpro )
    include_directories( ${BRL_INCLUDE_DIR}/bpro/core )

    include_directories( ${MUL_INCLUDE_DIR} )
    include_directories( ${BRL_INCLUDE_DIR}/bseg/brec )
    include_directories( ${BRL_INCLUDE_DIR}/bseg/bvxm )

    set(brec_batch_sources
        reg_brec.cxx          reg_brec.h
       )
    if(NOT VXL_BUILD_POSITION_DEPENDENT_CODE)
      vxl_add_library(LIBRARY_NAME brec_batch LIBRARY_SOURCES SHARED ${brec_batch_sources})

      if(WIN32)
        #library has to have different name depending on debug or release version.
        set_target_properties(brec_batch PROPERTIES OUTPUT_NAME brec_batch DEBUG_POSTFIX _d SUFFIX .pyd)
      endif()
      set_target_properties(brec_batch PROPERTIES PREFIX "")

      target_link_libraries(brec_batch bprb brdb bpro_batch bmdl_pro bvxm_pro bvxm_io bvxm_grid_io brec_pro bbgm_pro bvgl_pro brip_pro bsvg_pro ${VXL_LIB_PREFIX}vbl ${VXL_LIB_PREFIX}vpgl_io vidl_pro vpgl_pro ${VXL_LIB_PREFIX}vil_io vil_pro)

      if(Python3_LIBRARIES)
        target_link_libraries( brec_batch optimized ${Python3_LIBRARIES})
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
