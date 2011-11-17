# This is contrib/brl/bseg/bmdl/pro/CMakeLists.txt
# bmdl's Process Library

INCLUDE( ${VXL_CMAKE_DIR}/FindZLIB.cmake )
IF(ZLIB_FOUND)
  INCLUDE_DIRECTORIES( ${ZLIB_INCLUDE_DIR} )
  INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/b3p )
  ADD_DEFINITIONS(-DHAS_ZLIB=1)
ENDIF(ZLIB_FOUND)

# bprb depends on expat and bmdl_prodepends on bprb primarily
INCLUDE( ${BRL_MODULE_PATH}/FindEXPAT.cmake )
IF(EXPAT_FOUND)
  INCLUDE(${VXL_CMAKE_DIR}/FindTIFF.cmake)
  IF(TIFF_FOUND)
    INCLUDE( ${VXL_CMAKE_DIR}/FindGEOTIFF.cmake)
    IF(GEOTIFF_FOUND)
      INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR} )
      INCLUDE_DIRECTORIES( ${GEL_INCLUDE_DIR}/mrc )
      INCLUDE_DIRECTORIES( ${GEL_INCLUDE_DIR} )
      INCLUDE_DIRECTORIES( ${TIFF_INCLUDE_DIR} )
      INCLUDE_DIRECTORIES( ${GEOTIFF_INCLUDE_DIR} )

      SET(bmdl_pro_sources
          bmdl_processes.h
          bmdl_defs.h
          bmdl_register.h                  bmdl_register.cxx
         )

      AUX_SOURCE_DIRECTORY(Templates bmdl_pro_sources)
      AUX_SOURCE_DIRECTORY(processes bmdl_pro_sources)

      ADD_LIBRARY(bmdl_pro ${bmdl_pro_sources})

      TARGET_LINK_LIBRARIES(bmdl_pro bmdl bprb vil vil_io vpgl_file_formats vgl_io
                            vsol brip vpgl_pro vgl vul vpl imesh_algo minizip)

      IF(ZLIB_FOUND)
         TARGET_LINK_LIBRARIES( bmdl_pro minizip )
      ENDIF(ZLIB_FOUND)

      IF( BUILD_TESTING )
        SUBDIRS(tests)
      ENDIF( BUILD_TESTING )

    ENDIF(GEOTIFF_FOUND)
  ENDIF(TIFF_FOUND)
ENDIF(EXPAT_FOUND)
