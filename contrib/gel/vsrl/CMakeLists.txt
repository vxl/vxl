# ./gel/vsrl/CMakeLists.txt

ADD_LIBRARY(vsrl vsrl_sources)

SOURCE_FILES(vsrl_sources
  vsrl_3d_output.cxx
  vsrl_dense_matcher.cxx
  vsrl_diffusion.cxx
  vsrl_dynamic_program.cxx
  vsrl_image_correlation.cxx
  vsrl_intensity_token.cxx
  vsrl_null_token.cxx
  vsrl_parameters.cxx
  vsrl_raster_dp_setup.cxx
  vsrl_results_dense_matcher.cxx
  vsrl_saliency_diffusion.cxx
  vsrl_step_diffusion.cxx
  vsrl_stereo_dense_matcher.cxx
  vsrl_token.cxx
  vsrl_token_saliency.cxx
  vsrl_window_accumulator.cxx
)

AUX_SOURCE_DIRECTORY(Templates vsrl_sources)

IF (WIN32)
  IF (NOT CYGWIN)
    INCLUDE(${allvxl_SOURCE_DIR}/gel/vsrl/CMakeListsHeaders.txt)
  ENDIF (NOT CYGWIN)
ENDIF(WIN32)

IF( BUILD_EXAMPLES)
  SUBDIRS(examples)
ENDIF( BUILD_EXAMPLES)
