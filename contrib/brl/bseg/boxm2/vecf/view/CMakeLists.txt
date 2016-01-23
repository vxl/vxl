# boxm opencl opengl display
include( ${VXL_CMAKE_DIR}/NewCMake/FindOpenCL.cmake )
if(OPENCL_FOUND)
  add_definitions(-DHAS_OPENCL=1)
  include_directories( ${OPENCL_INCLUDE_PATH} )
  include( ${BRL_MODULE_PATH}/FindGLEW.cmake )
if(GLEW_FOUND)
  add_definitions(-DHAS_GLEW=1)

if(VGUI_FOUND)
  include_directories(${GLEW_INCLUDE_DIR} )
  include_directories( ${BRL_INCLUDE_DIR}/bpro )
  include_directories( ${BRL_INCLUDE_DIR}/bseg )
  include_directories( ${BRL_INCLUDE_DIR}/bbas )
  include_directories( ${GEL_INCLUDE_DIR}/mrc/vpgl )
  include_directories( ${GEL_INCLUDE_DIR}/mrc )
  include_directories( ${BRL_INCLUDE_DIR}/bseg/boxm2 )
  link_directories(${OPENGL_LIBRARY_DIR})

  set(boxm2_vecf_view_sources
      boxm2_ocl_articulated_render_tableau.h        boxm2_ocl_articulated_render_tableau.cxx
      boxm2_vecf_orbit_tableau.h                     boxm2_vecf_orbit_tableau.cxx
     )
  aux_source_directory(Templates boxm2_vecf_view_sources)

  add_library(boxm2_vecf_view ${boxm2_vecf_view_sources} )

  #install the .h .txx and libs
  install_targets(/lib boxm2_vecf_view)
  INSTALL_NOBASE_HEADER_FILES(/include/vxl/contrib/brl/bseg/boxm2/vecf/view ${boxm2_vecf_view_sources})

  target_link_libraries(boxm2_vecf_view bocl boxm2_pro boxm2_ocl_pro boxm2_cpp boxm2_cpp_pro vgui bprb vnl vnl_algo vbl_io vpgl vsph vgl vil vbl vul ${GLEW_LIBRARY} ${OPENCL_LIBRARIES} )

  #pro
#  add_subdirectory(pro)

  #tests
  if( BUILD_TESTING )
    add_subdirectory(tests)
  endif()
endif()
endif()
endif()
