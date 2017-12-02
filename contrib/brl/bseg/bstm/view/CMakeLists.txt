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
  vxl_add_library(LIBRARY_NAME bstm_view LIBRARY_SOURCES ${bstm_view_sources} )

  target_link_libraries(bstm_view bocl boxm2_view bstm_ocl_pro ${VXL_LIB_PREFIX}vgui bprb ${VXL_LIB_PREFIX}vnl ${VXL_LIB_PREFIX}vnl_algo ${VXL_LIB_PREFIX}vbl_io ${VXL_LIB_PREFIX}vpgl vsph ${VXL_LIB_PREFIX}vgl ${VXL_LIB_PREFIX}vil ${VXL_LIB_PREFIX}vbl ${VXL_LIB_PREFIX}vul ${GLEW_LIBRARY} ${OPENCL_LIBRARIES} )

  if( BUILD_TESTING )
    add_subdirectory(tests)
  endif()

endif()
endif()
endif()
