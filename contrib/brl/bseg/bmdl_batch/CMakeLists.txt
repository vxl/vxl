# contrib/brl/bseg/bmdl_batch/CMakeLists.txt
include( ${VXL_CMAKE_DIR}/NewCMake/FindPython.cmake )
if( PYTHON_FOUND )
  include( ${BRL_MODULE_PATH}/FindEXPAT.cmake )
  if( EXPAT_FOUND )
   if(EXPATPP_FOUND)
    include(${VXL_CMAKE_DIR}/FindTIFF.cmake)
    if(TIFF_FOUND)
      include( ${VXL_CMAKE_DIR}/FindGEOTIFF.cmake)
      if(GEOTIFF_FOUND)
        include( ${VXL_CMAKE_DIR}/FindZLIB.cmake )
        if(ZLIB_FOUND)
          include_directories( ${ZLIB_INCLUDE_DIR} )
          include_directories( ${BRL_INCLUDE_DIR}/b3p )
        endif()
        include_directories( ${PYTHON_INCLUDE_DIRS} )
        include_directories( ${BRL_INCLUDE_DIR} )
        include_directories( ${BRL_INCLUDE_DIR}/bpro )
        include_directories( ${BRL_INCLUDE_DIR}/bpro/core )

        include_directories( ${GEL_INCLUDE_DIR}/mrc )
        include_directories( ${MUL_INCLUDE_DIR} )
        include_directories( ${BRL_INCLUDE_DIR}/bseg/bmdl )

        include_directories( ${TIFF_INCLUDE_DIR} )
        include_directories( ${GEOTIFF_INCLUDE_DIR} )

        set(bmdl_batch_sources
            reg_bmdl.cxx          reg_bmdl.h
           )
        if(NOT VXL_BUILD_POSITION_DEPENDENT_CODE)
          vxl_add_library(LIBRARY_NAME bmdl_batch LIBRARY_SOURCES SHARED ${bmdl_batch_sources})

          if(WIN32)
            #library has to have different name depending on debug or release version.
            set_target_properties(bmdl_batch PROPERTIES OUTPUT_NAME bmdl_batch DEBUG_POSTFIX _d SUFFIX .pyd)
          endif()
          set_target_properties(bmdl_batch PROPERTIES PREFIX "")

          target_link_libraries(bmdl_batch bprb brdb bpro_batch bmdl_pro ${VXL_LIB_PREFIX}vbl ${VXL_LIB_PREFIX}vpgl_io vpgl_pro ${VXL_LIB_PREFIX}vil_io vil_pro expat expatpp)

          if(PYTHON_DEBUG_LIBRARIES)
            target_link_libraries( bmdl_batch debug ${PYTHON_DEBUG_LIBRARY})
          endif()

          if(PYTHON_LIBRARIES)
            target_link_libraries( bmdl_batch optimized ${PYTHON_LIBRARY})
          endif()

          if( BUILD_TESTING )
            add_subdirectory(tests)
          endif()
        endif()
      endif()
    endif()
   endif()
  endif()
endif()
