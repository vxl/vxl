# This is contrib/brl/bseg/bmdl/pro/CMakeLists.txt
# bmdl's Process Library

INCLUDE( ${MODULE_PATH}/FindZLIB.cmake )
IF(ZLIB_FOUND)
  INCLUDE_DIRECTORIES( ${ZLIB_INCLUDE_DIR} )
  INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/b3p )
  ADD_DEFINITIONS(-DHAS_ZLIB=1)
ENDIF(ZLIB_FOUND)

# bprb depends on expat and bmdl_prodepends on bprb primarily
INCLUDE( ${BRL_MODULE_PATH}/FindEXPAT.cmake )
IF(EXPAT_FOUND)
  INCLUDE(${MODULE_PATH}/FindTIFF.cmake)
  IF(TIFF_FOUND)
    INCLUDE( ${MODULE_PATH}/FindGEOTIFF.cmake)
    IF(GEOTIFF_FOUND)
      INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR} )
      INCLUDE_DIRECTORIES( ${GEL_INCLUDE_DIR}/mrc )
      INCLUDE_DIRECTORIES( ${GEL_INCLUDE_DIR} )
      INCLUDE_DIRECTORIES( ${TIFF_INCLUDE_DIR} )
      INCLUDE_DIRECTORIES( ${GEOTIFF_INCLUDE_DIR} )
      
      SET(bmdl_pro_sources
        bmdl_classify_process.h               bmdl_classify_process.cxx
        bmdl_trace_boundaries_process.h       bmdl_trace_boundaries_process.cxx
        bmdl_generate_mesh_process.h          bmdl_generate_mesh_process.cxx
        bmdl_lidar_roi_process.h              bmdl_lidar_roi_process.cxx
        bmdl_modeling_process.h               bmdl_modeling_process.cxx
      )


      AUX_SOURCE_DIRECTORY(Templates bmdl_pro_sources)

      ADD_LIBRARY(bmdl_pro ${bmdl_pro_sources})

      TARGET_LINK_LIBRARIES(bmdl_pro bmdl bprb brdb vil vil_io vpgl_file_formats vgl_io
                            bgeo vsol brip vpgl_pro vpl imesh_algo minizip)

                            
      IF(ZLIB_FOUND)
         TARGET_LINK_LIBRARIES( bmdl_pro minizip ) 
      ENDIF(ZLIB_FOUND)
         
      IF( BUILD_TESTING )
        SUBDIRS(tests)
      ENDIF( BUILD_TESTING )
  
    ENDIF(GEOTIFF_FOUND)
  ENDIF(TIFF_FOUND)
ENDIF(EXPAT_FOUND)
