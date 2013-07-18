# contrib/brl/bseg/bstm_batch/CMakeLists.txt
PROJECT( bstm_batch )

INCLUDE( ${VXL_CMAKE_DIR}/NewCMake/FindPython.cmake )
INCLUDE( ${VXL_CMAKE_DIR}/NewCMake/FindOpenCL.cmake )
INCLUDE( ${BRL_MODULE_PATH}/FindGLEW.cmake )
INCLUDE( ${VXL_CMAKE_DIR}/UseVGUI.cmake )

INCLUDE( ${VXL_CMAKE_DIR}/NewCMake/FindDirectShow.cmake )
IF(DIRECTSHOW_FOUND)
  ADD_DEFINITIONS(-DHAS_DIRECTSHOW)
ENDIF(DIRECTSHOW_FOUND)

IF( PYTHON_FOUND )
  INCLUDE( ${BRL_MODULE_PATH}/FindEXPAT.cmake )
  IF( EXPAT_FOUND )
    INCLUDE_DIRECTORIES( ${PYTHON_INCLUDE_DIRS} )
    INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR} )
    INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bpro )
    INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bpro/core )

    INCLUDE_DIRECTORIES( ${VXLCORE_INCLUDE_DIR} )
    INCLUDE_DIRECTORIES( ${GEL_INCLUDE_DIR}/mrc )
    INCLUDE_DIRECTORIES( ${MUL_INCLUDE_DIR} )
    INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bseg )
    INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bbas)

     SET(bstm_batch_sources
         reg_bstm.h   reg_bstm.cxx
        )

    IF(NOT BUILD_POSITION_DEPENDENT_CODE)
      ADD_LIBRARY(bstm_batch SHARED ${bstm_batch_sources})

      #library has to have different name depending on debug or release version.
      IF(WIN32)
        SET_TARGET_PROPERTIES(bstm_batch PROPERTIES OUTPUT_NAME bstm_batch DEBUG_POSTFIX _d SUFFIX .pyd)
      ENDIF(WIN32)
      IF(APPLE)
        SET_TARGET_PROPERTIES(bstm_batch PROPERTIES OUTPUT_NAME bstm_batch SUFFIX .so)
      ENDIF(APPLE)
      SET_TARGET_PROPERTIES(bstm_batch PROPERTIES PREFIX "")

      TARGET_LINK_LIBRARIES(bstm_batch bstm_pro bstm_cpp_pro bpro_batch boxm2_pro boxm2_cpp_pro vpgl_pro vil_pro ihog_pro brip_pro bsvg_pro bvrml_pro bbgm_pro sdet_pro brad_pro brad_io brdb icam_pro bvpl_kernels_pro)

      IF(OPENCL_FOUND)
        ADD_DEFINITIONS(-DHAS_OPENCL=1)
        TARGET_LINK_LIBRARIES(bstm_batch bocl_pro bstm_ocl bstm_ocl_pro boxm2_ocl_pro)
      ENDIF(OPENCL_FOUND)
      
      
      IF (PYTHON_DEBUG_LIBRARIES)
        TARGET_LINK_LIBRARIES( bstm_batch debug ${PYTHON_DEBUG_LIBRARY})
      ENDIF(PYTHON_DEBUG_LIBRARIES )

      IF(PYTHON_LIBRARIES)
        TARGET_LINK_LIBRARIES( bstm_batch optimized ${PYTHON_LIBRARY})
      ENDIF(PYTHON_LIBRARIES)
      
      
      #install the .h .txx and libs
      INSTALL_TARGETS(/lib bstm_batch)
      INSTALL_NOBASE_HEADER_FILES(/include/vxl/contrib/brl/bseg/bstm_batch/ ${bstm_batch_sources})

      IF( BUILD_TESTING )
        SUBDIRS(tests)
      ENDIF( BUILD_TESTING )
    ENDIF (NOT BUILD_POSITION_DEPENDENT_CODE)
  ENDIF( EXPAT_FOUND )
ENDIF( PYTHON_FOUND )


