# This is contrib/brl/bseg/bmdl/pro/CMakeLists.txt
# bmdl's Process Library

include( ${VXL_CMAKE_DIR}/FindZLIB.cmake )
if(ZLIB_FOUND)
  include_directories( ${ZLIB_INCLUDE_DIR} )
  include_directories( ${BRL_INCLUDE_DIR}/b3p )
  add_definitions(-DHAS_ZLIB=1)
endif()

# bprb depends on expat and bmdl_prodepends on bprb primarily
include( ${BRL_MODULE_PATH}/FindEXPAT.cmake )
if(EXPAT_FOUND)
  include(${VXL_CMAKE_DIR}/FindTIFF.cmake)
  if(TIFF_FOUND)
    include( ${VXL_CMAKE_DIR}/FindGEOTIFF.cmake)
    if(GEOTIFF_FOUND)
      include_directories( ${BRL_INCLUDE_DIR} )
      include_directories( ${GEL_INCLUDE_DIR}/mrc )
      include_directories( ${GEL_INCLUDE_DIR} )
      include_directories( ${TIFF_INCLUDE_DIR} )
      include_directories( ${GEOTIFF_INCLUDE_DIR} )

      set(bmdl_pro_sources
          bmdl_processes.h
          bmdl_defs.h
          bmdl_register.h                  bmdl_register.cxx
         )

      aux_source_directory(Templates bmdl_pro_sources)
      aux_source_directory(processes bmdl_pro_sources)

      vxl_add_library(LIBRARY_NAME bmdl_pro LIBRARY_SOURCES ${bmdl_pro_sources})

      target_link_libraries(bmdl_pro bmdl bprb ${VXL_LIB_PREFIX}vil ${VXL_LIB_PREFIX}vil_io ${VXL_LIB_PREFIX}vpgl_file_formats ${VXL_LIB_PREFIX}vgl_io vsol brip vpgl_pro ${VXL_LIB_PREFIX}vgl ${VXL_LIB_PREFIX}vul ${VXL_LIB_PREFIX}vpl imesh_algo minizip)

      if(ZLIB_FOUND)
         target_link_libraries( bmdl_pro minizip )
      endif()

      if( BUILD_TESTING )
        add_subdirectory(tests)
      endif()

    endif()
  endif()
endif()
