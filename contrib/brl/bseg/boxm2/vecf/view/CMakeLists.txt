# boxm opencl opengl display
if(OpenCL_FOUND)
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

  vxl_add_library(LIBRARY_NAME boxm2_vecf_view LIBRARY_SOURCES ${boxm2_vecf_view_sources} )

  #install the .h .hxx and libs

  target_link_libraries(boxm2_vecf_view bocl boxm2_pro boxm2_ocl_pro boxm2_cpp boxm2_cpp_pro ${VXL_LIB_PREFIX}vgui bprb ${VXL_LIB_PREFIX}vnl ${VXL_LIB_PREFIX}vnl_algo ${VXL_LIB_PREFIX}vbl_io ${VXL_LIB_PREFIX}vpgl vsph ${VXL_LIB_PREFIX}vgl ${VXL_LIB_PREFIX}vil ${VXL_LIB_PREFIX}vbl ${VXL_LIB_PREFIX}vul ${GLEW_LIBRARY} ${OpenCL_LIBRARIES} )

  #pro
#  add_subdirectory(pro)

  #tests
  if( BUILD_TESTING )
    add_subdirectory(tests)
  endif()
endif()
endif()
endif()
