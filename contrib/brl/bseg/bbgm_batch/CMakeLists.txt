# contrib/brl/bseg/bbgm_batch/CMakeLists.txt
include( ${VXL_CMAKE_DIR}/NewCMake/FindPython.cmake )
if( PYTHON_FOUND )
  include( ${BRL_MODULE_PATH}/FindEXPAT.cmake )
  if( EXPAT_FOUND )
    if(EXPATPP_FOUND)
    include(${VXL_CMAKE_DIR}/FindTIFF.cmake)
    if(TIFF_FOUND)
      include( ${VXL_CMAKE_DIR}/FindGEOTIFF.cmake)
      if(HAS_GEOTIFF)
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
        include_directories( ${BRL_INCLUDE_DIR}/bseg/bbgm )

        include_directories( ${TIFF_INCLUDE_DIR} )
        include_directories( ${GEOTIFF_INCLUDE_DIR} )

        set(bbgm_batch_sources
            reg_bbgm.cxx          reg_bbgm.h
           )

        if(NOT VXL_BUILD_POSITION_DEPENDENT_CODE)
          vxl_add_library(LIBRARY_NAME bbgm_batch LIBRARY_SOURCES SHARED ${bbgm_batch_sources})

          if(WIN32)
            #library has to have different name depending on debug or release version.
            set_target_properties(bbgm_batch PROPERTIES OUTPUT_NAME bbgm_batch DEBUG_POSTFIX _d SUFFIX .pyd)
          endif()

          set_target_properties(bbgm_batch PROPERTIES PREFIX "")

          target_link_libraries(bbgm_batch
            bprb brdb bpro_batch bbgm_pro
            vidl_pro vil_pro
            vbl vil_io vil_io expat expatpp)

          if(PYTHON_DEBUG_LIBRARIES)
            target_link_libraries( bbgm_batch debug ${PYTHON_DEBUG_LIBRARY})
          endif()

          if(PYTHON_LIBRARIES)
            target_link_libraries( bbgm_batch optimized ${PYTHON_LIBRARY})
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
