# brl/bseg/boxm/algo/CMakeLists.txt
include_directories( ${MUL_INCLUDE_DIR} )
set(boxm_algo_rt_sources
    boxm_change_probability_functor.h
    boxm_edge_tangent_updater.h             boxm_edge_tangent_updater.txx
    boxm_edge_updater.h                     boxm_edge_updater.txx
    boxm_generate_edge_sample_functor.h
    boxm_generate_edge_tangent_sample_functor.h
    boxm_generate_opt2_sample_functor.h
    boxm_generate_opt3_sample_functor.h
    boxm_generate_opt_sample_functor.h
    boxm_opt2_optimizer.h                   boxm_opt2_optimizer.txx
    boxm_opt3_optimizer.h                   boxm_opt3_optimizer.txx
    boxm_opt_appearance_estimator.h
    boxm_opt_rt_bayesian_optimizer.h        boxm_opt_rt_bayesian_optimizer.txx
    boxm_pre_infinity_functor.h
    boxm_render_expected_depth_functor.h
    boxm_render_expected_edge_image_functor.h
    boxm_render_expected_edge_tangent_image_functor.h
    boxm_render_expected_image_functor.h
    boxm_render_expected_shadow_image_functor.h
    boxm_rpc_registration.h
    boxm_sun_vis_prob_functor.h
    boxm_update_image_functor.h
    boxm_compute_num_rays_statistics.h
    boxm_expected_edge_functor.h
    boxm_shadow_appearance_estimator.h
    boxm_generate_shadow_sample_functor.h
    boxm_shadow_app_initializer.h            boxm_shadow_app_initializer.txx
    boxm_shadow_bayes_optimizer.h            boxm_shadow_bayes_optimizer.txx
    dummy.cxx
   )

aux_source_directory(Templates boxm_algo_rt_sources)

add_library(boxm_algo_rt ${boxm_algo_rt_sources})

#processes
add_subdirectory(pro)

if( BUILD_TESTING )
  add_subdirectory(tests)
endif( BUILD_TESTING )

target_link_libraries(boxm_algo_rt boxm boxm_sample boxm_sample_algo boxm_basic boxm_util vil vcl vgl vbl vgl_algo bsta vnl sdet brip vpgl vpgl_algo )

