# This is brl/bsta/algo/CMakeLists.txt
# The Brown Statistics Algorithms Library

SET( bsta_algo_sources
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
   )

AUX_SOURCE_DIRECTORY(Templates bsta_algo_sources)

ADD_LIBRARY( bsta_algo ${bsta_algo_sources} )
TARGET_LINK_LIBRARIES(bsta_algo bsta vnl_algo vnl vbl)

IF( BUILD_TESTING )
  SUBDIRS(tests)
ENDIF( BUILD_TESTING )

INSTALL_TARGETS(/lib bsta_algo)
INSTALL_NOBASE_HEADER_FILES(/include/vxl/contrib/brl/bbas/bsta/algo ${bsta_algo_sources})
