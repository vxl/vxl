#depends on OPENCL being found...
INCLUDE_DIRECTORIES( ${OPENCL_INCLUDE_PATH} )
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bbas )
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR} )
SET(boxm2_ocl_algo_sources
    boxm2_ocl_render_expected_image.h           boxm2_ocl_render_expected_image.cxx
    boxm2_ocl_render_expected_image_function.h  boxm2_ocl_render_expected_image_function.cxx
    boxm2_ocl_camera_converter.h                boxm2_ocl_camera_converter.cxx
    boxm2_ocl_cone_update_function.h            boxm2_ocl_cone_update_function.cxx
    boxm2_ocl_paint_batch.h                     boxm2_ocl_paint_batch.cxx
    boxm2_ocl_paint_online.h                    boxm2_ocl_paint_online.cxx
    boxm2_ocl_paint_online_color.h              boxm2_ocl_paint_online_color.cxx
    boxm2_ocl_change_detection.h                boxm2_ocl_change_detection.cxx
    boxm2_ocl_update_color.h                    boxm2_ocl_update_color.cxx
    boxm2_ocl_refine.h                          boxm2_ocl_refine.cxx
    boxm2_ocl_render_scene_uncertainty_map.h    boxm2_ocl_render_scene_uncertainty_map.cxx
    boxm2_ocl_update.h                          boxm2_ocl_update.cxx
    boxm2_ocl_update_based_dispersion.h         boxm2_ocl_update_based_dispersion.cxx
    boxm2_ocl_update_view_dep_app.h             boxm2_ocl_update_view_dep_app.cxx
    boxm2_ocl_update_view_dep_app_color.h       boxm2_ocl_update_view_dep_app_color.cxx
    boxm2_ocl_update_auxQ.h						boxm2_ocl_update_auxQ.cxx
    boxm2_ocl_update_sky.h                      boxm2_ocl_update_sky.cxx
    boxm2_ocl_update_max_vis.h                  boxm2_ocl_update_max_vis.cxx
    boxm2_ocl_update_vis_score.h                boxm2_ocl_update_vis_score.cxx
    boxm2_ocl_fuse_based_visibility.h           boxm2_ocl_fuse_based_visibility.cxx
    boxm2_ocl_expected_image_renderer.h         boxm2_ocl_expected_image_renderer.cxx
    boxm2_ocl_depth_renderer.h                  boxm2_ocl_depth_renderer.cxx
    boxm2_ocl_num_obs_renderer.h                boxm2_ocl_num_obs_renderer.cxx
    boxm2_ocl_vis_score_renderer.h                boxm2_ocl_vis_score_renderer.cxx
    boxm2_ocl_update_sun_visibilities.h         boxm2_ocl_update_sun_visibilities.cxx
    boxm2_ocl_render_expected_shadow_map.h      boxm2_ocl_render_expected_shadow_map.cxx
    boxm2_ocl_refine_scene_around_geometry.h boxm2_ocl_refine_scene_around_geometry.cxx
    boxm2_ocl_filter_scene_data.h               boxm2_ocl_filter_scene_data.cxx
    boxm2_ocl_aggregate_normal_from_filter_vector.h  boxm2_ocl_aggregate_normal_from_filter_vector.cxx
    boxm2_ocl_kernel_vector_filter.h            boxm2_ocl_kernel_vector_filter.cxx
   )

ADD_LIBRARY(boxm2_ocl_algo  ${boxm2_ocl_algo_sources})
TARGET_LINK_LIBRARIES(boxm2_ocl_algo boxm2_ocl vgl vil vul vnl_algo brad bvpl_kernels)

#tests
IF( BUILD_TESTING )
  SUBDIRS(tests)
ENDIF( BUILD_TESTING )

INSTALL_TARGETS(/lib boxm2_ocl_algo)
INSTALL_NOBASE_HEADER_FILES(/include/vxl/contrib/brl/bseg/boxm2/ocl/algo ${boxm2_ocl_algo_sources})
