# basic open_cl library

include( ${VXL_CMAKE_DIR}/NewCMake/FindOpenCL.cmake )
if(OPENCL_FOUND)
  add_definitions(-DHAS_OPENCL=1)
  include_directories( ${OPENCL_INCLUDE_PATH} )
  include_directories( ${BRL_INCLUDE_DIR}/bbas )

  # some calls may be different for versions prior to 1.2 (e.g. clCreateImage2D)
  CMAKE_DEPENDENT_OPTION(VXL_OPENCL_IS_PRE_V1.2 "OpenCL library version prior to 1.2" ON "VXL_BUILD_BRL;OPENCL_FOUND" OFF)
  if(VXL_OPENCL_IS_PRE_V1.2)
    add_definitions(-DOPENCL_IS_PRE_V12)
  endif()

  set(bocl_sources
      bocl_manager.h      bocl_manager.hxx  bocl_manager.cxx
      bocl_cl.h
      bocl_cl_gl.h
      bocl_device_info.h  bocl_device_info.cxx
      bocl_device.h       bocl_device.cxx
      bocl_utils.h        bocl_utils.cxx
      bocl_mem.h          bocl_mem.cxx
      bocl_kernel.h       bocl_kernel.cxx
  )
  aux_source_directory(Templates bocl_sources)
  vxl_add_library(LIBRARY_NAME bocl LIBRARY_SOURCES ${bocl_sources} )

  target_link_libraries(bocl brdb ${VXL_LIB_PREFIX}vnl ${VXL_LIB_PREFIX}vnl_algo ${VXL_LIB_PREFIX}vbl_io ${VXL_LIB_PREFIX}vpgl ${VXL_LIB_PREFIX}vgl ${VXL_LIB_PREFIX}vil ${VXL_LIB_PREFIX}vbl ${VXL_LIB_PREFIX}vul ${OPENCL_LIBRARIES})

  #install the .h .hxx and libs

#tests
if( BUILD_TESTING )
  add_subdirectory(tests)
endif()

#pro
add_subdirectory(pro)
endif()
