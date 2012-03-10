USES += contrib/brl contrib/brl/bbas contrib/brl/bseg core

LIBRARY := boxm2_ocl_pro

SOURCES += processes/boxm2_ocl_change_detection_process.cxx

SOURCES += boxm2_ocl_register.cxx
MANPAGE_SOURCES += boxm2_ocl_processes.h
SOURCES += processes/boxm2_ocl_render_expected_image_process.cxx
SOURCES += processes/boxm2_create_opencl_cache_process.cxx
SOURCES += processes/boxm2_ocl_query_hist_data_process.cxx
SOURCES += processes/boxm2_ocl_update_histogram_process.cxx
SOURCES += processes/boxm2_ocl_batch_probability_process.cxx
SOURCES += processes/boxm2_ocl_query_cell_data_process.cxx
SOURCES += processes/boxm2_ocl_render_expected_depth_process.cxx
SOURCES += processes/boxm2_ocl_update_process.cxx
SOURCES += processes/boxm2_ocl_refine_process.cxx
SOURCES += processes/boxm2_ocl_render_gl_expected_image_process.cxx
SOURCES += processes/boxm2_ocl_render_expected_height_map_process.cxx
SOURCES += processes/boxm2_ocl_update_color_process.cxx
SOURCES += processes/boxm2_ocl_render_expected_color_process.cxx
SOURCES += processes/boxm2_ocl_render_gl_expected_color_process.cxx
SOURCES += processes/boxm2_ocl_convert_float_image_to_rgba_process.cxx
SOURCES += processes/boxm2_ocl_probability_of_image_process.cxx
SOURCES += processes/boxm2_ocl_probability_of_image_gl_process.cxx
SOURCES += processes/boxm2_ocl_render_cone_expected_image_process.cxx
SOURCES += processes/boxm2_ocl_filter_process.cxx
SOURCES += processes/boxm2_clear_opencl_cache_process.cxx
SOURCES += processes/boxm2_ocl_cone_update_process.cxx
SOURCES += processes/boxm2_ocl_update_aux_per_view_process.cxx
SOURCES += processes/boxm2_ocl_adaptive_cone_update_process.cxx
SOURCES += processes/boxm2_ocl_adaptive_cone_render_expected_process.cxx
SOURCES += processes/boxm2_ocl_merge_process.cxx
SOURCES += processes/boxm2_ocl_render_expected_shadow_map_process.cxx
SOURCES += processes/boxm2_ocl_update_sun_visibilities_process.cxx
SOURCES += processes/boxm2_ocl_aux_update_view_direction_process.cxx
SOURCES += processes/boxm2_ocl_create_aux_process.cxx
SOURCES += processes/boxm2_ocl_create_norm_intensities_process.cxx
SOURCES += processes/boxm2_ocl_ingest_dem_process.cxx
SOURCES += processes/boxm2_ocl_visualize_change_process.cxx
SOURCES += processes/boxm2_ocl_paint_batch_process.cxx
SOURCES += processes/boxm2_ocl_batch_synoptic_function_process.cxx
SOURCES += processes/boxm2_ocl_flip_normals_using_vis_process.cxx
SOURCES += processes/boxm2_ocl_paint_online_process.cxx
SOURCES += processes/boxm2_ocl_render_expected_albedo_normal_process.cxx
SOURCES += processes/boxm2_ocl_ingest_buckeye_process.cxx
SOURCES += processes/boxm2_ocl_update_alpha_naa_process.cxx
SOURCES += processes/boxm2_ocl_render_expected_z_image_process.cxx
SOURCES += processes/boxm2_ocl_render_expected_image_naa_process.cxx
SOURCES += processes/boxm2_ocl_update_alpha_wcubic_process.cxx
SOURCES += processes/boxm2_ocl_batch_uncertainty_process.cxx
SOURCES += processes/boxm2_ocl_batch_compute_normal_albedo_array_process.cxx
SOURCES += processes/boxm2_ocl_update_aux_per_view_naa_process.cxx
SOURCES += processes/boxm2_ocl_update_with_alt_process.cxx
SOURCES += processes/boxm2_ocl_uncertainty_per_image_process.cxx
SOURCES += processes/boxm2_ocl_aggregate_normal_from_filter_response_process.cxx

#######################################
include ${VXLROOT}/config/top-params.mk
#######################################

ifndef NOTEST
SUBDIRS += tests
endif

######################################
include ${VXLROOT}/config/top-rules.mk
######################################
