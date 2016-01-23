# This is brl/bsta/algo/CMakeLists.txt
# The Brown Statistics Algorithms Library

set( bsta_algo_sources
   bsta_gaussian_stats.h
   bsta_gaussian_updater.h
   bsta_mixture_functors.h
   bsta_bayes_functor.h
   bsta_adaptive_updater.txx  bsta_adaptive_updater.h
   bsta_truth_updater.txx     bsta_truth_updater.h
   bsta_parzen_updater.txx    bsta_parzen_updater.h
   bsta_von_mises_updater.txx bsta_von_mises_updater.h
   bsta_fit_weibull.cxx       bsta_fit_weibull.h
   bsta_mean_shift.txx        bsta_mean_shift.h
   bsta_sample_set.txx        bsta_sample_set.h
   bsta_beta_updater.txx      bsta_beta_updater.h
   bsta_fit_gaussian.h
   bsta_sigma_normalizer.h    bsta_sigma_normalizer.cxx
   bsta_display_vrml.h
   bsta_mvnrand.h
   )

aux_source_directory(Templates bsta_algo_sources)

add_library( bsta_algo ${bsta_algo_sources} )
target_link_libraries(bsta_algo bsta vnl_algo vnl vbl)

if( BUILD_TESTING )
  add_subdirectory(tests)
endif( BUILD_TESTING )

install_targets(/lib bsta_algo)
INSTALL_NOBASE_HEADER_FILES(/include/vxl/contrib/brl/bbas/bsta/algo ${bsta_algo_sources})
