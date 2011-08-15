# basic radiometery library

  INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bbas )

  SET(brad_sources
      brad_illum_util.h        brad_illum_util.cxx brad_illum_cost_function.h
      brad_sun_pos.h           brad_sun_pos.cxx
      brad_sun_dir_index.h     brad_sun_dir_index.cxx
      brad_hist_prob_feature_vector.h    brad_hist_prob_feature_vector.cxx
      brad_grad_hist_feature_vector.h    brad_grad_hist_feature_vector.cxx
      brad_grad_int_feature_vector.h    brad_grad_int_feature_vector.cxx
      brad_eigenspace.h        brad_eigenspace.txx 
      brad_eigenspace_base.h   brad_eigenspace_base.cxx 
      brad_eigenspace_sptr.h
      brad_phongs_model_est.h  brad_phongs_model_est.cxx
      brad_lambertian_model_est.h brad_lambertian_model_est.cxx
  )
  AUX_SOURCE_DIRECTORY(Templates brad_sources)
  ADD_LIBRARY(brad ${brad_sources} )

  TARGET_LINK_LIBRARIES(brad  bsta vnl_io vnl vnl_algo vgl vil vbl_io vbl vpl vul )

#pro
SUBDIRS(pro)

#tests
SUBDIRS(tests)

