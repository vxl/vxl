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
      boxm_refine_manager.h boxm_refine_manager.txx
      boxm_ocl_refine_scene.h
      boxm_ocl_utils.h boxm_ocl_utils.txx boxm_ocl_utils.cxx
      boxm_ocl_render_expected.h        boxm_ocl_render_expected.cxx
      boxm_stat_manager.h boxm_stat_manager.txx
      boxm_refine_scene_manager.h boxm_refine_scene_manager.cxx
      boxm_ocl_scene.h boxm_ocl_scene.cxx
      boxm_render_ocl_scene_manager.h boxm_render_ocl_scene_manager.cxx
      boxm_re_render_ocl_scene_manager.h boxm_re_render_ocl_scene_manager.cxx
      boxm_update_ocl_scene_manager.h boxm_update_ocl_scene_manager.cxx
      boxm_ocl_change_detection_manager.h boxm_ocl_change_detection_manager.cxx boxm_ocl_change_detection_manager_sptr.h
      boxm_ocl_camera_manager.h boxm_ocl_camera_manager.cxx
      boxm_ocl_bit_scene.h boxm_ocl_bit_scene.cxx
      boxm_render_bit_scene_manager.h boxm_render_bit_scene_manager.cxx
      boxm_render_probe_manager.h boxm_render_probe_manager.cxx boxm_render_probe_manager_sptr.h
      boxm_update_bit_scene_manager.h boxm_update_bit_scene_manager.cxx
     )

  SET(boxm_ocl_cl_files
      cl/backproject.cl
      cl/bit_tree_library_functions.cl
      cl/cell_utils.cl
      cl/change_detection_ocl_scene.cl
      cl/expected_functor.cl
      cl/loc_code_library_functions.cl
      cl/octree_library_functions.cl
      cl/ray_bundle_library_functions.cl
      cl/ray_trace_bit_scene.cl
      cl/ray_trace_bit_scene_float3.cl
      cl/ray_trace_bit_scene_opt.cl
      cl/ray_trace_main.cl
      cl/ray_trace_ocl_scene.cl
      cl/refine_blocks.cl
      cl/refine_blocks_opt.cl
      cl/refine_main.cl
      cl/refine_main_opt.cl
      cl/render_bit_scene.cl
      cl/rerender.cl
      cl/scene_info.cl
      cl/statistics_library_functions.cl
      cl/update_main.cl
      cl/update_ocl_scene.cl
     )
  AUX_SOURCE_DIRECTORY(Templates boxm_ocl_sources)
  ADD_LIBRARY(boxm_ocl ${boxm_ocl_sources} ${boxm_ocl_cl_files})

  TARGET_LINK_LIBRARIES(boxm_ocl bocl boxm boxm_util boxm_sample boxm_basic boct bprb bgeo vnl vnl_algo vbl_io vpgl vgl vil vbl vul vsl vnl_io ${OPENCL_LIBRARIES})

  SUBDIRS(tests)
  SUBDIRS(pro)
  SUBDIRS(exe)
  SUBDIRS(view)

ENDIF(OPENCL_FOUND)
