#bstm/ocl source dir

#depends on OPENCL being found...
include( ${VXL_CMAKE_DIR}/NewCMake/FindOpenCL.cmake )
if(OPENCL_FOUND)

message("BSTM FOUND OPENCL")

add_definitions(-DHAS_OPENCL=1)
include_directories( ${OPENCL_INCLUDE_PATH} )
include_directories( ${BRL_INCLUDE_DIR}/bbas )

set(bstm_ocl_sources
    bstm_ocl_util.h         bstm_ocl_util.cxx
    bstm_opencl_cache.h     bstm_opencl_cache.cxx
    bstm_scene_info.h
   )

aux_source_directory(Templates bstm_ocl_sources)

vxl_add_library(LIBRARY_NAME bstm_ocl LIBRARY_SOURCES ${bstm_ocl_sources})
target_link_libraries(bstm_ocl bstm bstm_io bocl ${VXL_LIB_PREFIX}vpgl ${VXL_LIB_PREFIX}vil ${VXL_LIB_PREFIX}vcl)

if( BUILD_TESTING )
  add_subdirectory(tests)
endif()

#pro
add_subdirectory(pro)

#algo
add_subdirectory(algo)

#exe
add_subdirectory(exe)

endif()
