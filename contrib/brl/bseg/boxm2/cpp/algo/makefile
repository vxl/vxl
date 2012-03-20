USES += contrib/brl contrib/brl/bbas contrib/brl/bseg core

LIBRARY := boxm2_cpp_algo

SOURCES += boxm2_mog3_grey_processor.cxx
MANPAGE_SOURCES += boxm2_refine_block_function.h
MANPAGE_SOURCES += boxm2_render_exp_image_functor.h
MANPAGE_SOURCES += boxm2_cast_ray_function.h
MANPAGE_SOURCES += boxm2_data_serial_iterator.h
MANPAGE_SOURCES += boxm2_update_image_functor.h
MANPAGE_SOURCES += boxm2_change_detection_functor.h
SOURCES += boxm2_refine_block_function.cxx
SOURCES += boxm2_render_functions.cxx
MANPAGE_SOURCES += boxm2_render_cone_functor.h
MANPAGE_SOURCES += boxm2_cast_cone_ray_function.h
MANPAGE_SOURCES += boxm2_render_exp_depth_functor.h
MANPAGE_SOURCES += boxm2_cast_intensities_functor.h
MANPAGE_SOURCES += boxm2_mean_intensities_batch_functor.h
SOURCES += boxm2_filter_block_function.cxx
MANPAGE_SOURCES += boxm2_batch_functors.h
SOURCES += boxm2_update_functions.cxx
MANPAGE_SOURCES += boxm2_ray_probe_functor.h
MANPAGE_SOURCES += boxm2_cone_update_image_functor.h
SOURCES += boxm2_gauss_grey_processor.cxx
SOURCES += boxm2_merge_block_function.cxx
MANPAGE_SOURCES += boxm2_batch_opt2_functors.h
MANPAGE_SOURCES += boxm2_update_with_shadow_functor.h
MANPAGE_SOURCES += boxm2_image_density_functor.h
SOURCES += boxm2_phongs_model_processor.cxx
MANPAGE_SOURCES += boxm2_compute_phongs_model_functor.h
MANPAGE_SOURCES += boxm2_batch_opt2_phongs_functors.h
MANPAGE_SOURCES += boxm2_shadow_model_functor.h
MANPAGE_SOURCES += boxm2_compute_nonsurface_histogram_functor.h
MANPAGE_SOURCES += boxm2_update_using_quality_functor.h
MANPAGE_SOURCES += boxm2_synoptic_function_functors.h
SOURCES += boxm2_export_point_cloud_xyz_function.cxx
SOURCES += boxm2_compute_derivative_function.cxx
MANPAGE_SOURCES += boxm2_apply_filter_function.txx
MANPAGE_SOURCES += boxm2_compute_normal_albedo_functor.h
SOURCES += boxm2_points_to_volume_function.cxx
SOURCES += boxm2_compute_normal_albedo_functor.cxx
SOURCES += boxm2_compute_normal_albedo_functor_opt.cxx
SOURCES += boxm2_gauss_rgb_processor.cxx

#######################################
include ${VXLROOT}/config/top-params.mk
#######################################

ifndef NOTEST
 SUBDIRS += tests
endif

######################################
include ${VXLROOT}/config/top-rules.mk
######################################
