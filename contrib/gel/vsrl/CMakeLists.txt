

ADD_LIBRARY(vsrl vsrl_sources)

SOURCE_FILES(vsrl_sources
vsrl_3d_output          vsrl_raster_dp_setup
vsrl_dense_matcher      vsrl_results_dense_matcher
vsrl_diffusion          vsrl_saliency_diffusion
vsrl_dynamic_program    vsrl_step_diffusion
vsrl_image_correlation  vsrl_stereo_dense_matcher
vsrl_intensity_token    vsrl_token
vsrl_null_token         vsrl_token_saliency
vsrl_parameters         vsrl_window_accumulator
)

AUX_SOURCE_DIRECTORY(Templates vsrl_sources)