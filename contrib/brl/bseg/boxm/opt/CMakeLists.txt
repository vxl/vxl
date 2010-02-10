# brl/bseg/boxm/opt/CMakeLists.txt

INCLUDE_DIRECTORIES( ${GEL_INCLUDE_DIR}/mrc )

SET(boxm_opt_sources
   boxm_aux_traits.h
   boxm_aux_scene.txx                   boxm_aux_scene.h
   boxm_opt_rt_bayesian_optimizer.txx   boxm_opt_rt_bayesian_optimizer.h
   boxm_opt_appearance_estimator.h
   boxm_raytrace_function.h
   boxm_generate_opt_sample_functor.h
   boxm_render_expected_depth_functor.h
   boxm_render_expected_image_functor.h
   boxm_seg_length_functor.h
   boxm_pre_infinity_functor.h
   boxm_rt_sample.h                     boxm_rt_sample.txx
   boxm_update_image_functor.h
   boxm_edge_updater.h                  boxm_edge_updater.txx
   boxm_generate_edge_sample_functor.h
   boxm_render_expected_edge_image_functor.h
   boxm_rpc_registration.h
   boxm_generate_edge_tangent_sample_functor.h
   boxm_edge_tangent_updater.h          boxm_edge_tangent_updater.txx
)

AUX_SOURCE_DIRECTORY(Templates boxm_opt_sources)

ADD_LIBRARY(boxm_opt ${boxm_opt_sources})

TARGET_LINK_LIBRARIES(boxm_opt boxm vil vgl vpl vbl sdet)

#processes
SUBDIRS(pro)

#tests
SUBDIRS(tests)

#open_cl parallel computation
SUBDIRS(open_cl)
