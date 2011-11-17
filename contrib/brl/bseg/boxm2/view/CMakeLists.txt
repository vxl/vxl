# boxm opencl opengl display
INCLUDE( ${VXL_CMAKE_DIR}/NewCMake/FindOpenCL.cmake )
IF(OPENCL_FOUND)
  ADD_DEFINITIONS(-DHAS_OPENCL=1)
  INCLUDE_DIRECTORIES( ${OPENCL_INCLUDE_PATH} )
  INCLUDE( ${BRL_MODULE_PATH}/FindGLEW.cmake )
IF(GLEW_FOUND)
  ADD_DEFINITIONS(-DHAS_GLEW=1)

IF(VGUI_FOUND)
  INCLUDE_DIRECTORIES(${GLEW_INCLUDE_DIR} )
  INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bpro )
  INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bseg )
  INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bbas )
  INCLUDE_DIRECTORIES( ${GEL_INCLUDE_DIR}/mrc/vpgl )
  INCLUDE_DIRECTORIES( ${GEL_INCLUDE_DIR}/mrc )
  LINK_DIRECTORIES(${OPENGL_LIBRARY_DIR})

  SET(boxm2_view_sources
      boxm2_cam_tableau.h               boxm2_cam_tableau.cxx
      boxm2_ocl_render_tableau.h        boxm2_ocl_render_tableau.cxx
      boxm2_ocl_update_tableau.h        boxm2_ocl_update_tableau.cxx
      boxm2_ocl_change_tableau.h        boxm2_ocl_change_tableau.cxx
      boxm2_trajectory.h
      boxm2_trajectory.cxx
      boxm2_include_glew.h
      boxm2_view_utils.h                boxm2_view_utils.cxx
     )
  AUX_SOURCE_DIRECTORY(Templates boxm2_view_sources)

  ADD_LIBRARY(boxm2_view ${boxm2_view_sources} )

  #install the .h .txx and libs
  INSTALL_TARGETS(/lib boxm2_view)
  INSTALL_NOBASE_HEADER_FILES(/include/vxl/contrib/brl/bseg/boxm2/view ${boxm2_view_sources})

  TARGET_LINK_LIBRARIES(boxm2_view bocl boxm2_pro boxm2_ocl_pro boxm2_cpp boxm2_cpp_pro vgui bprb vnl vnl_algo vbl_io vpgl vsph vgl vil vbl vul ${GLEW_LIBRARY} ${OPENCL_LIBRARIES} )

  #pro
  SUBDIRS(pro)

  #tests
  IF( BUILD_TESTING )
    SUBDIRS(tests)
  ENDIF( BUILD_TESTING )
ENDIF(VGUI_FOUND)
ENDIF(GLEW_FOUND)
ENDIF(OPENCL_FOUND)
