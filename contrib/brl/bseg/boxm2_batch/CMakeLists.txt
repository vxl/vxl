# contrib/brl/bseg/boxm2_batch/CMakeLists.txt
if (HAS_GEOTIFF)
project( boxm2_batch )

include( ${VXL_CMAKE_DIR}/NewCMake/FindPython.cmake )
include( ${VXL_CMAKE_DIR}/NewCMake/FindOpenCL.cmake )
include( ${BRL_MODULE_PATH}/FindGLEW.cmake )
include( ${BRL_CMAKE_DIR}/FindHadoop.cmake )
include( ${VXL_CMAKE_DIR}/UseVGUI.cmake )

if( PYTHON_FOUND )
  include( ${BRL_MODULE_PATH}/FindEXPAT.cmake )
  if( EXPAT_FOUND )
    include_directories( ${PYTHON_INCLUDE_DIRS} )
    include_directories( ${BRL_INCLUDE_DIR} )
    include_directories( ${BRL_INCLUDE_DIR}/bpro )
    include_directories( ${BRL_INCLUDE_DIR}/bpro/core )

    include_directories( ${GEL_INCLUDE_DIR}/mrc )
    include_directories( ${MUL_INCLUDE_DIR} )
    include_directories( ${BRL_INCLUDE_DIR}/bseg )
    include_directories( ${BRL_INCLUDE_DIR}/bbas)

    if(OPENCL_FOUND)
      include_directories( ${OPENCL_INCLUDE_PATH} )
    endif()

     set(boxm2_batch_sources
         reg_boxm2.h   reg_boxm2.cxx
        )
    if(NOT VXL_BUILD_POSITION_DEPENDENT_CODE)
      vxl_add_library(LIBRARY_NAME boxm2_batch LIBRARY_SOURCES SHARED ${boxm2_batch_sources})

      #library has to have different name depending on debug or release version.
      if(WIN32)
        set_target_properties(boxm2_batch PROPERTIES OUTPUT_NAME boxm2_batch DEBUG_POSTFIX _d SUFFIX .pyd)
      endif()
      if(APPLE)
        set_target_properties(boxm2_batch PROPERTIES OUTPUT_NAME boxm2_batch SUFFIX .so)
      endif()
      set_target_properties(boxm2_batch PROPERTIES PREFIX "")

      target_link_libraries(boxm2_batch bpro_batch boxm2_pro boxm2_cpp_pro vpgl_pro vil_pro bvgl_pro ihog_pro brip_pro bsvg_pro bvrml_pro bbgm_pro sdet_pro brad_pro brad_io brdb icam_pro bvpl_kernels_pro bsgm_pro)
      if(VXL_BOXM2_USE_VOLM)
        add_definitions(-DUSE_VOLM=1)
        target_link_libraries(boxm2_batch volm_pro)
      endif()
      if(GLEW_FOUND AND HAS_VGUI AND OPENCL_FOUND)
        add_definitions(-DHAS_GLEW=1)
        target_link_libraries(boxm2_batch boxm2_view_pro )
      endif()

      if(OPENCL_FOUND)
        add_definitions(-DHAS_OPENCL=1)
        target_link_libraries(boxm2_batch bocl_pro  boxm2_ocl_pro boxm2_multi_pro boxm2_vecf_ocl_pro)
        if(VXL_BOXM2_USE_VOLM)
             target_link_libraries(boxm2_batch boxm2_volm_pro boxm2_volm_io )
        endif()

      endif()

      if(HDFS_FOUND)
        add_definitions(-DHAS_HDFS=1)
        target_link_libraries(boxm2_batch bhdfs_pro )
      endif()

      if(PYTHON_DEBUG_LIBRARIES)
        target_link_libraries( boxm2_batch debug ${PYTHON_DEBUG_LIBRARY})
      endif()

      if(PYTHON_LIBRARIES)
        target_link_libraries( boxm2_batch optimized ${PYTHON_LIBRARY})
      endif()



      if( BUILD_TESTING )
        add_subdirectory(tests)
      endif()

      #install the .h .hxx and libs

    endif()
  endif()
endif()

include( ${VXL_CMAKE_DIR}/NewCMake/FindDirectShow.cmake )
if(DIRECTSHOW_FOUND)
  add_definitions(-DHAS_DIRECTSHOW)
endif()
else()
  message(STATUS "Skipping contrib/brl/bseg/boxm2_batch: requires geotiff")
endif()
