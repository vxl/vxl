# contrib/brl/bseg/bstm_batch/CMakeLists.txt
if (HAS_GEOTIFF)
project( bstm_batch )

include( ${VXL_CMAKE_DIR}/UseVGUI.cmake )

include( ${VXL_CMAKE_DIR}/NewCMake/FindDirectShow.cmake )
if(DIRECTSHOW_FOUND)
  add_definitions(-DHAS_DIRECTSHOW)
endif()

if( Python3_Development_FOUND )
  include( ${BRL_MODULE_PATH}/FindEXPAT.cmake )
  if( EXPAT_FOUND )
    include_directories( ${Python3_INCLUDE_DIRS} )
    include_directories( ${BRL_INCLUDE_DIR} )
    include_directories( ${BRL_INCLUDE_DIR}/bpro )
    include_directories( ${BRL_INCLUDE_DIR}/bpro/core )

    include_directories( ${GEL_INCLUDE_DIR}/mrc )
    include_directories( ${MUL_INCLUDE_DIR} )
    include_directories( ${BRL_INCLUDE_DIR}/bseg )
    include_directories( ${BRL_INCLUDE_DIR}/bbas)

     set(bstm_batch_sources
         reg_bstm.h   reg_bstm.cxx
        )

    if(NOT VXL_BUILD_POSITION_DEPENDENT_CODE)
      vxl_add_library(LIBRARY_NAME bstm_batch LIBRARY_SOURCES SHARED ${bstm_batch_sources})

      #library has to have different name depending on debug or release version.
      if(WIN32)
        set_target_properties(bstm_batch PROPERTIES OUTPUT_NAME bstm_batch DEBUG_POSTFIX _d SUFFIX .pyd)
      endif()
      if(APPLE)
        set_target_properties(bstm_batch PROPERTIES OUTPUT_NAME bstm_batch SUFFIX .so)
      endif()
      set_target_properties(bstm_batch PROPERTIES PREFIX "")

      target_link_libraries(bstm_batch bstm_pro bstm_cpp_pro bpro_batch boxm2_pro boxm2_cpp_pro vpgl_pro vil_pro ihog_pro brip_pro bsvg_pro bvrml_pro bbgm_pro sdet_pro brad_pro brad_io brdb icam_pro bvpl_kernels_pro)

      if(OpenCL_FOUND)
        target_link_libraries(bstm_batch bocl_pro bstm_ocl bstm_ocl_pro boxm2_ocl_pro)
      endif()

      if(Python3_LIBRARIES)
        target_link_libraries( bstm_batch optimized ${Python3_LIBRARIES})
      endif()


      #install the .h .hxx and libs

      if( BUILD_TESTING )
        add_subdirectory(tests)
      endif()
    endif()
  endif()
endif()
else()
  message(STATUS "Skipping contrib/brl/bseg/bstm_batch: requires geotiff")
endif()


