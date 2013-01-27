INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bseg )
INCLUDE_DIRECTORIES( ${MUL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VTHREEP_INCLUDE_DIR}/rply)

SET(boxm2_cpp_algo_sources
    boxm2_cast_ray_function.h
    boxm2_cast_cone_ray_function.h   #boxm2_cast_adaptive_cone_ray_function.h
    boxm2_render_functions.h          boxm2_render_functions.cxx
    boxm2_render_exp_image_functor.h
    boxm2_render_exp_depth_functor.h
    boxm2_render_cone_functor.h
    boxm2_update_image_functor.h
    boxm2_update_functions.h          boxm2_update_functions.cxx
    boxm2_cone_update_image_functor.h
    boxm2_refine_block_function.h     boxm2_refine_block_function.cxx
    boxm2_change_detection_functor.h
    boxm2_mog3_grey_processor.h       boxm2_mog3_grey_processor.cxx
    boxm2_gauss_grey_processor.h      boxm2_gauss_grey_processor.cxx
    boxm2_gauss_rgb_processor.h       boxm2_gauss_rgb_processor.cxx
    boxm2_phongs_model_processor.h    boxm2_phongs_model_processor.cxx
    boxm2_mog6_view_processor.h

    boxm2_data_serial_iterator.h
    boxm2_cast_intensities_functor.h
    boxm2_mean_intensities_batch_functor.h
    boxm2_filter_block_function.h     boxm2_filter_block_function.cxx
    boxm2_compute_derivative_function.h     boxm2_compute_derivative_function.cxx
    boxm2_apply_filter_function.h     boxm2_apply_filter_function.txx
    boxm2_export_oriented_point_cloud_function.h     boxm2_export_oriented_point_cloud_function.cxx
    boxm2_batch_functors.h
    boxm2_ray_probe_functor.h
    boxm2_vis_probe_functor.h
    boxm2_shadow_model_functor.h
    boxm2_batch_opt2_functors.h
    boxm2_batch_opt2_phongs_functors.h
    boxm2_synoptic_function_functors.h
    boxm2_image_density_functor.h
    boxm2_update_with_shadow_functor.h
    boxm2_update_using_quality_functor.h
    boxm2_merge_block_function.h     boxm2_merge_block_function.cxx

    boxm2_compute_nonsurface_histogram_functor.h
    boxm2_compute_phongs_model_functor.h
    boxm2_compute_normal_albedo_functor.h  boxm2_compute_normal_albedo_functor.cxx
    boxm2_compute_normal_albedo_functor_opt.h  boxm2_compute_normal_albedo_functor_opt.cxx
    boxm2_points_to_volume_function.h  boxm2_points_to_volume_function.cxx
    boxm2_export_stack_images_function.h boxm2_export_stack_images_function.cxx
    boxm2_normals_to_id_functor.h
   )
AUX_SOURCE_DIRECTORY(Templates boxm2_cpp_algo_sources)

ADD_LIBRARY(boxm2_cpp_algo  ${boxm2_cpp_algo_sources})
TARGET_LINK_LIBRARIES(boxm2_cpp_algo
                      boxm2_cpp brad boct brdb expatpp vpgl bvgl imesh imesh_algo bsta_algo bsta vil_algo vil vgl_xio vgl vnl_algo vnl vul vbl_io vbl vsl vcl bvpl rply)

IF(BUILD_TESTING)
  SUBDIRS(tests)
ENDIF(BUILD_TESTING)
