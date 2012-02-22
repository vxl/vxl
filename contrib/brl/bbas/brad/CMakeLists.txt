# basic radiometery library

  INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bbas )

  SET(brad_sources
      brad_illum_util.h               brad_illum_util.cxx
      brad_illum_cost_function.h
      brad_sun_pos.h                  brad_sun_pos.cxx
      brad_sun_dir_index.h            brad_sun_dir_index.cxx
      brad_hist_prob_feature_vector.h brad_hist_prob_feature_vector.cxx
      brad_grad_hist_feature_vector.h brad_grad_hist_feature_vector.cxx
      brad_grad_int_feature_vector.h  brad_grad_int_feature_vector.cxx
      brad_eigenspace.h               brad_eigenspace.txx
      brad_eigenspace_base.h          brad_eigenspace_base.cxx
      brad_eigenspace_sptr.h
      brad_phongs_model_est.h         brad_phongs_model_est.cxx
      brad_synoptic_function_1d.h     brad_synoptic_function_1d.cxx
      brad_image_metadata.h           brad_image_metadata.cxx
      brad_atmospheric_parameters.h   brad_atmospheric_parameters.cxx 
      brad_image_atmospherics_est.h   brad_image_atmospherics_est.cxx
      brad_estimate_shadows.h         brad_estimate_shadows.cxx
     )
  AUX_SOURCE_DIRECTORY(Templates brad_sources)
  ADD_LIBRARY(brad ${brad_sources} )

  TARGET_LINK_LIBRARIES(brad  bsta vnl_io vnl vgl_algo vnl_algo vgl vil vpgl vpgl_algo vbl_io vbl vpl vul )
  
  SUBDIRS(io)

#tests
IF( BUILD_TESTING )
  SUBDIRS(tests)
ENDIF( BUILD_TESTING )

INSTALL_TARGETS(/lib brad)
INSTALL_NOBASE_HEADER_FILES(/include/vxl/contrib/brl/bbas/brad ${brad_sources})
