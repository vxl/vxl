# this is an executable to run a process for profiling purposes
ADD_EXECUTABLE( boxm_opencl_render_expected_process
  boxm_opencl_render_expected_process_exe.cxx
 )
TARGET_LINK_LIBRARIES( boxm_opencl_render_expected_process boxm_pro  boxm_ocl boxm_ocl_pro vil_pro vpgl_pro boxm_algo_rt_pro vul )

INCLUDE( ${BRL_MODULE_PATH}/FindGLEW.cmake )
INCLUDE( ${VXL_CMAKE_DIR}/NewCMake/FindGLUT.cmake )




IF(GLEW_FOUND)
	IF(GLUT_FOUND)
	INCLUDE_DIRECTORIES( ${GLEW_INCLUDE_DIR} )
	LINK_DIRECTORIES(${OPENGL_LIBRARY_DIR}) 

	ADD_EXECUTABLE( boxm_ocl_render_with_gl
	  boxm_ocl_render_with_gl.cxx
	 )
	TARGET_LINK_LIBRARIES( boxm_ocl_render_with_gl boxm_pro  boxm_ocl boxm_ocl_pro vil_pro vpgl_pro boxm_algo_rt_pro vul ${GLEW_LIBRARY} ${GLUT_glut_LIBRARY})
	ENDIF(GLUT_FOUND)
ENDIF(GLEW_FOUND)
