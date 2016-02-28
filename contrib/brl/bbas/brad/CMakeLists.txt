# basic radiometery library

  include_directories( ${BRL_INCLUDE_DIR}/bbas )

  set(brad_sources
      brad_illum_util.h               brad_illum_util.cxx
      brad_illum_cost_function.h
      brad_sun_pos.h                  brad_sun_pos.cxx
      brad_sun_dir_index.h            brad_sun_dir_index.cxx
      brad_hist_prob_feature_vector.h brad_hist_prob_feature_vector.cxx
      brad_grad_hist_feature_vector.h brad_grad_hist_feature_vector.cxx
      brad_grad_int_feature_vector.h  brad_grad_int_feature_vector.cxx
      brad_eigenspace.h               brad_eigenspace.hxx
      brad_eigenspace_base.h          brad_eigenspace_base.cxx
      brad_eigenspace_sptr.h
      brad_phongs_model_est.h         brad_phongs_model_est.cxx
      brad_synoptic_function_1d.h     brad_synoptic_function_1d.cxx
      brad_image_metadata.h           brad_image_metadata.cxx
      brad_atmospheric_parameters.h   brad_atmospheric_parameters.cxx
      brad_image_atmospherics_est.h   brad_image_atmospherics_est.cxx
      brad_estimate_shadows.h         brad_estimate_shadows.cxx
     )
  aux_source_directory(Templates brad_sources)
  vxl_add_library( LIBNAME brad LIBSRCS ${brad_sources} )

  target_link_libraries(brad  bsta ${VXL_LIB_PREFIX}vnl_io ${VXL_LIB_PREFIX}vnl ${VXL_LIB_PREFIX}vgl_algo ${VXL_LIB_PREFIX}vgl_io ${VXL_LIB_PREFIX}vnl_algo ${VXL_LIB_PREFIX}vgl ${VXL_LIB_PREFIX}vil ${VXL_LIB_PREFIX}vpgl ${VXL_LIB_PREFIX}vpgl_algo ${VXL_LIB_PREFIX}vbl_io ${VXL_LIB_PREFIX}vbl ${VXL_LIB_PREFIX}vpl ${VXL_LIB_PREFIX}vul )

  add_subdirectory(io)

#tests
if( BUILD_TESTING )
  add_subdirectory(tests)
endif()

