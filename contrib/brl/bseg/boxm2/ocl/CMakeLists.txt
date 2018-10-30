#boxm2/ocl source dir
if(VXL_BUILD_BRL)
  set(BOXM2_OPENCL_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/cl CACHE STRING "Location of OpenCL source files for brl/bseg/boxm2/ (defaults to source tree)")
endif()

# Set up boxm2_ocl_where_root_dir.h to have a macro set to $BOXM2_OPENCL_SRC_DIR
vxl_configure_file(${CMAKE_CURRENT_LIST_DIR}/boxm2_ocl_where_root_dir.h.in
  ${PROJECT_BINARY_DIR}/bseg/boxm2/ocl/boxm2_ocl_where_root_dir.h contrib/brl/bseg/boxm2/ocl
)
include_directories( ${PROJECT_BINARY_DIR}/bseg/boxm2/ocl/ )


#depends on OPENCL being found...
include( ${VXL_CMAKE_DIR}/NewCMake/FindOpenCL.cmake )
if(OPENCL_FOUND)
  add_definitions(-DHAS_OPENCL=1)
  include_directories( ${OPENCL_INCLUDE_PATH} )
  include_directories( ${BRL_INCLUDE_DIR}/bbas )

  set(boxm2_ocl_sources
      boxm2_opencl_cache.h     boxm2_opencl_cache.cxx
      boxm2_opencl_cache1.h     boxm2_opencl_cache1.cxx
      boxm2_ocl_util.h         boxm2_ocl_util.cxx
    )
  aux_source_directory(Templates boxm2_ocl_sources)

  vxl_add_library(LIBRARY_NAME boxm2_ocl LIBRARY_SOURCES ${boxm2_ocl_sources})
  target_link_libraries(boxm2_ocl boxm2 boxm2_basic boxm2_io bocl boct bsta brdb expatpp ${VXL_LIB_PREFIX}vpgl ${VXL_LIB_PREFIX}vgl_xio ${VXL_LIB_PREFIX}vgl ${VXL_LIB_PREFIX}vil ${VXL_LIB_PREFIX}vnl_algo ${VXL_LIB_PREFIX}vnl ${VXL_LIB_PREFIX}vbl_io ${VXL_LIB_PREFIX}vbl ${VXL_LIB_PREFIX}vsl ${VXL_LIB_PREFIX}vcl)

  #install the .h .hxx and libs

  #pro
  add_subdirectory(pro)

  #exe
  add_subdirectory(exe)

  #algo
  add_subdirectory(algo)

  #tests
  if( BUILD_TESTING )
    add_subdirectory(tests)
  endif()

endif()
