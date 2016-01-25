# basic open_cl library

include( ${VXL_CMAKE_DIR}/NewCMake/FindOpenCL.cmake )
if(OPENCL_FOUND)
  add_definitions(-DHAS_OPENCL=1)
  include_directories( ${OPENCL_INCLUDE_PATH} )
  include_directories( ${BRL_INCLUDE_DIR}/bbas )

  # some calls may be different for versions prior to 1.2 (e.g. clCreateImage2D)
  CMAKE_DEPENDENT_OPTION(OPENCL_IS_PRE_V1.2 "OpenCL library version prior to 1.2" ON "BUILD_BRL;OPENCL_FOUND" OFF)
  if(OPENCL_IS_PRE_V1.2)
    add_definitions(-DOPENCL_IS_PRE_V12)
  endif()

  set(bocl_sources
      bocl_manager.h      bocl_manager.txx  bocl_manager.cxx
      bocl_cl.h
      bocl_cl_gl.h
      bocl_device_info.h  bocl_device_info.cxx
      bocl_device.h       bocl_device.cxx
      bocl_utils.h        bocl_utils.cxx
      bocl_mem.h          bocl_mem.cxx
      bocl_kernel.h       bocl_kernel.cxx
  )
  aux_source_directory(Templates bocl_sources)
  add_library(bocl ${bocl_sources} )

  target_link_libraries(bocl brdb vnl vnl_algo vbl_io vpgl vgl vil vbl vul ${OPENCL_LIBRARIES})

  #install the .h .txx and libs
  install_targets(/lib bocl)
  INSTALL_NOBASE_HEADER_FILES(/include/vxl/contrib/brl/bbas/bocl/ ${bocl_sources})

#tests
if( BUILD_TESTING )
  add_subdirectory(tests)
endif()

#pro
add_subdirectory(pro)
endif()
