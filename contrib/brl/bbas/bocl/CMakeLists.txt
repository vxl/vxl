# basic open_cl library
if(OpenCL_FOUND)

  include_directories( ${BRL_INCLUDE_DIR}/bbas )

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

  target_link_libraries(bocl brdb ${VXL_LIB_PREFIX}vnl ${VXL_LIB_PREFIX}vnl_algo ${VXL_LIB_PREFIX}vbl_io ${VXL_LIB_PREFIX}vpgl ${VXL_LIB_PREFIX}vgl ${VXL_LIB_PREFIX}vil ${VXL_LIB_PREFIX}vbl ${VXL_LIB_PREFIX}vul ${OpenCL_LIBRARIES})

  #install the .h .hxx and libs

#tests
if( BUILD_TESTING )
  add_subdirectory(tests)
endif()

#pro
add_subdirectory(pro)
endif()
