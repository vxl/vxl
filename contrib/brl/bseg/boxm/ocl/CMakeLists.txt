# boxm open_cl acceleration library

INCLUDE( ${BRL_MODULE_PATH}/find_open_cl.cmake )
IF(OPENCL_FOUND)
  ADD_DEFINITIONS(-DHAS_OPENCL=1)
  INCLUDE_DIRECTORIES( ${OPENCL_INCLUDE_PATH} )
  INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bseg )
  INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bbas )
  INCLUDE_DIRECTORIES( ${GEL_INCLUDE_DIR}/mrc/vpgl )
  INCLUDE_DIRECTORIES( ${GEL_INCLUDE_DIR}/mrc )

  SET(boxm_ocl_sources
      boxm_ray_trace_manager.h boxm_ray_trace_manager.txx
      boxm_stack_ray_trace_manager.h boxm_stack_ray_trace_manager.txx
	  boxm_online_update_manager.h boxm_online_update_manager.txx
      boxm_ray_bundle_trace_manager.h boxm_ray_bundle_trace_manager.txx
      boxm_refine_manager.h boxm_refine_manager.txx
      boxm_ocl_refine_scene.h
      boxm_ocl_utils.h boxm_ocl_utils.txx boxm_ocl_utils.cxx
      boxm_ocl_render_expected.h
      boxm_stat_manager.h boxm_stat_manager.txx
     )

  SET(bomx_ocl_cl_files
      octree_library_functions.cl
      ray_trace_main.cl
      ray_stack_trace_main.cl
      refine_main.cl
      expected_functor.cl
      ray_bundle_library_functions.cl
      update_main.cl
     )
  AUX_SOURCE_DIRECTORY(Templates boxm_ocl_sources)
  ADD_LIBRARY(boxm_ocl ${boxm_ocl_sources} ${bomx_ocl_cl_files})

  TARGET_LINK_LIBRARIES(boxm_ocl bocl boxm boxm_util boxm_sample boxm_basic boct bprb bgeo vnl vnl_algo vbl_io vpgl vgl vil vbl vul ${OPENCL_LIBRARIES})

  SUBDIRS(tests)
  SUBDIRS(pro)
  SUBDIRS(exe)

ENDIF(OPENCL_FOUND)
