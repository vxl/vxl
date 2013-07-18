# contrib/brl/bseg/bbgm_batch/CMakeLists.txt
INCLUDE( ${VXL_CMAKE_DIR}/NewCMake/FindPython.cmake )
IF( PYTHON_FOUND )
  INCLUDE( ${BRL_MODULE_PATH}/FindEXPAT.cmake )
  IF( EXPAT_FOUND )
    IF(EXPATPP_FOUND)
    INCLUDE(${VXL_CMAKE_DIR}/FindTIFF.cmake)
    IF(TIFF_FOUND)
      INCLUDE( ${VXL_CMAKE_DIR}/FindGEOTIFF.cmake)
      IF(GEOTIFF_FOUND)
        INCLUDE( ${VXL_CMAKE_DIR}/FindZLIB.cmake )
        IF(ZLIB_FOUND)
          INCLUDE_DIRECTORIES( ${ZLIB_INCLUDE_DIR} )
          INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/b3p )
        ENDIF(ZLIB_FOUND)
        INCLUDE_DIRECTORIES( ${PYTHON_INCLUDE_DIRS} )
        INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR} )
        INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bpro )
        INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bpro/core )

        INCLUDE_DIRECTORIES( ${VXLCORE_INCLUDE_DIR} )
        INCLUDE_DIRECTORIES( ${GEL_INCLUDE_DIR}/mrc )
        INCLUDE_DIRECTORIES( ${MUL_INCLUDE_DIR} )
        INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bseg/bbgm )

        INCLUDE_DIRECTORIES( ${TIFF_INCLUDE_DIR} )
        INCLUDE_DIRECTORIES( ${GEOTIFF_INCLUDE_DIR} )

        SET(bbgm_batch_sources
            reg_bbgm.cxx          reg_bbgm.h
           )

        IF(NOT BUILD_POSITION_DEPENDENT_CODE)
          ADD_LIBRARY(bbgm_batch SHARED ${bbgm_batch_sources})

          IF(WIN32)
            #library has to have different name depending on debug or release version.
            SET_TARGET_PROPERTIES(bbgm_batch PROPERTIES OUTPUT_NAME bbgm_batch DEBUG_POSTFIX _d SUFFIX .pyd)
          ENDIF(WIN32)

          SET_TARGET_PROPERTIES(bbgm_batch PROPERTIES PREFIX "")

          TARGET_LINK_LIBRARIES(bbgm_batch
            bprb brdb bpro_batch bbgm_pro
            vidl_pro vil_pro
            vbl vil_io vil_io expat expatpp)

          IF (PYTHON_DEBUG_LIBRARIES)
            TARGET_LINK_LIBRARIES( bbgm_batch debug ${PYTHON_DEBUG_LIBRARY})
          ENDIF(PYTHON_DEBUG_LIBRARIES )

          IF(PYTHON_LIBRARIES)
            TARGET_LINK_LIBRARIES( bbgm_batch optimized ${PYTHON_LIBRARY})
          ENDIF(PYTHON_LIBRARIES)

          IF( BUILD_TESTING )
            SUBDIRS(tests)
          ENDIF( BUILD_TESTING )
        ENDIF(NOT BUILD_POSITION_DEPENDENT_CODE)
      ENDIF(GEOTIFF_FOUND )
    ENDIF(TIFF_FOUND )
   ENDIF( EXPATPP_FOUND )
  ENDIF( EXPAT_FOUND )
ENDIF( PYTHON_FOUND )
