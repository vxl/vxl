# gel/vsrl/CMakeLists.txt

SET(vsrl_sources
 vsrl_3d_output.cxx             vsrl_3d_output.h
 vsrl_dense_matcher.cxx         vsrl_dense_matcher.h
 vsrl_diffusion.cxx             vsrl_diffusion.h
 vsrl_dynamic_program.cxx       vsrl_dynamic_program.h
 vsrl_image_correlation.cxx     vsrl_image_correlation.h
 vsrl_intensity_token.cxx       vsrl_intensity_token.h
 vsrl_null_token.cxx            vsrl_null_token.h
 vsrl_parameters.cxx            vsrl_parameters.h
 vsrl_raster_dp_setup.cxx       vsrl_raster_dp_setup.h
 vsrl_results_dense_matcher.cxx vsrl_results_dense_matcher.h
 vsrl_saliency_diffusion.cxx    vsrl_saliency_diffusion.h
 vsrl_step_diffusion.cxx        vsrl_step_diffusion.h
 vsrl_stereo_dense_matcher.cxx  vsrl_stereo_dense_matcher.h
 vsrl_token.cxx                 vsrl_token.h
 vsrl_token_saliency.cxx        vsrl_token_saliency.h
 vsrl_window_accumulator.cxx    vsrl_window_accumulator.h
)

AUX_SOURCE_DIRECTORY(Templates vsrl_sources)

ADD_LIBRARY(vsrl ${vsrl_sources})
TARGET_LINK_LIBRARIES(vsrl vil vbl vnl_algo vnl osl vcl)

IF (BUILD_EXAMPLES)
  SUBDIRS(examples)
ENDIF (BUILD_EXAMPLES)
