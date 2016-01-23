# contrib/brl/bseg/bstm_batch/CMakeLists.txt
project( bstm_batch )

include( ${VXL_CMAKE_DIR}/NewCMake/FindPython.cmake )
include( ${VXL_CMAKE_DIR}/NewCMake/FindOpenCL.cmake )
include( ${VXL_CMAKE_DIR}/UseVGUI.cmake )

include( ${VXL_CMAKE_DIR}/NewCMake/FindDirectShow.cmake )
if(DIRECTSHOW_FOUND)
  add_definitions(-DHAS_DIRECTSHOW)
endif(DIRECTSHOW_FOUND)

if( PYTHON_FOUND )
  include( ${BRL_MODULE_PATH}/FindEXPAT.cmake )
  if( EXPAT_FOUND )
    include_directories( ${PYTHON_INCLUDE_DIRS} )
    include_directories( ${BRL_INCLUDE_DIR} )
    include_directories( ${BRL_INCLUDE_DIR}/bpro )
    include_directories( ${BRL_INCLUDE_DIR}/bpro/core )

    include_directories( ${VXLCORE_INCLUDE_DIR} )
    include_directories( ${GEL_INCLUDE_DIR}/mrc )
    include_directories( ${MUL_INCLUDE_DIR} )
    include_directories( ${BRL_INCLUDE_DIR}/bseg )
    include_directories( ${BRL_INCLUDE_DIR}/bbas)

     set(bstm_batch_sources
         reg_bstm.h   reg_bstm.cxx
        )

    if(NOT BUILD_POSITION_DEPENDENT_CODE)
      add_library(bstm_batch SHARED ${bstm_batch_sources})

      #library has to have different name depending on debug or release version.
      if(WIN32)
        set_target_properties(bstm_batch PROPERTIES OUTPUT_NAME bstm_batch DEBUG_POSTFIX _d SUFFIX .pyd)
      endif(WIN32)
      if(APPLE)
        set_target_properties(bstm_batch PROPERTIES OUTPUT_NAME bstm_batch SUFFIX .so)
      endif(APPLE)
      set_target_properties(bstm_batch PROPERTIES PREFIX "")

      target_link_libraries(bstm_batch bstm_pro bstm_cpp_pro bpro_batch boxm2_pro boxm2_cpp_pro vpgl_pro vil_pro ihog_pro brip_pro bsvg_pro bvrml_pro bbgm_pro sdet_pro brad_pro brad_io brdb icam_pro bvpl_kernels_pro)

      if(OPENCL_FOUND)
        add_definitions(-DHAS_OPENCL=1)
        target_link_libraries(bstm_batch bocl_pro bstm_ocl bstm_ocl_pro boxm2_ocl_pro)
      endif(OPENCL_FOUND)


      if(PYTHON_DEBUG_LIBRARIES)
        target_link_libraries( bstm_batch debug ${PYTHON_DEBUG_LIBRARY})
      endif(PYTHON_DEBUG_LIBRARIES )

      if(PYTHON_LIBRARIES)
        target_link_libraries( bstm_batch optimized ${PYTHON_LIBRARY})
      endif(PYTHON_LIBRARIES)


      #install the .h .txx and libs
      install_targets(/lib bstm_batch)
      INSTALL_NOBASE_HEADER_FILES(/include/vxl/contrib/brl/bseg/bstm_batch/ ${bstm_batch_sources})

      if( BUILD_TESTING )
        add_subdirectory(tests)
      endif( BUILD_TESTING )
    endif(NOT BUILD_POSITION_DEPENDENT_CODE)
  endif( EXPAT_FOUND )
endif( PYTHON_FOUND )


