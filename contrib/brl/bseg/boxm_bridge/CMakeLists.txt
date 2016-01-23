include( ${VXL_CMAKE_DIR}/NewCMake/FindOpenCL.cmake )
if(OPENCL_FOUND)

  add_definitions(-DHAS_OPENCL=1)
  include_directories( ${OPENCL_INCLUDE_PATH} )
  include_directories( ${GEL_INCLUDE_DIR}/mrc )
  include_directories( ${MUL_INCLUDE_DIR} )

  add_executable( boxm2_to_boxm_exe boxm2_to_boxm.cxx  boxm2_to_boxm.h)
  target_link_libraries( boxm2_to_boxm_exe boxm2 boxm boxm_sample boxm2_io boct vnl vgl vul vbl vcl)

  # executable to convert a bit scene to a boxm2_scene
  add_executable( boxm_to_boxm2_exe boxm_to_boxm2.cxx  boxm_to_boxm2.h)
  target_link_libraries( boxm_to_boxm2_exe boxm2 boxm boxm_sample boxm2_io boct vnl vgl vul vbl vcl)

  #tests
  if( BUILD_TESTING )
    add_subdirectory(tests)
  endif( BUILD_TESTING )

endif(OPENCL_FOUND)
