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
  link_directories(${OPENGL_LIBRARY_DIR})

  set(bstm_view_sources
      bstm_include_glew.h
      bstm_cam_tableau.h        bstm_cam_tableau.cxx
      bstm_ocl_render_tableau.h  bstm_ocl_render_tableau.cxx
      bstm_ocl_multi_render_tableau.h  bstm_ocl_multi_render_tableau.cxx
     )
  add_library(bstm_view ${bstm_view_sources} )

  target_link_libraries(bstm_view bocl boxm2_view bstm_ocl_pro vgui bprb vnl vnl_algo vbl_io vpgl vsph vgl vil vbl vul ${GLEW_LIBRARY} ${OPENCL_LIBRARIES} )

  if( BUILD_TESTING )
    add_subdirectory(tests)
  endif()

endif()
endif()
endif()
