# This is contrib/brl/bseg/bmdl/pro/CMakeLists.txt
# bmdl's Process Library

include( ${VXL_CMAKE_DIR}/FindZLIB.cmake )
if(ZLIB_FOUND)
  include_directories( ${ZLIB_INCLUDE_DIR} )
  include_directories( ${BRL_INCLUDE_DIR}/b3p )
  add_definitions(-DHAS_ZLIB=1)
endif(ZLIB_FOUND)

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

      add_library(bmdl_pro ${bmdl_pro_sources})

      target_link_libraries(bmdl_pro bmdl bprb vil vil_io vpgl_file_formats vgl_io
                            vsol brip vpgl_pro vgl vul vpl imesh_algo minizip)

      if(ZLIB_FOUND)
         target_link_libraries( bmdl_pro minizip )
      endif(ZLIB_FOUND)

      if( BUILD_TESTING )
        add_subdirectory(tests)
      endif( BUILD_TESTING )

    endif(GEOTIFF_FOUND)
  endif(TIFF_FOUND)
endif(EXPAT_FOUND)
