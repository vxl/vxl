# This is brl/bsta/algo/CMakeLists.txt
# The Brown Statistics Algorithms Library

set( bsta_algo_sources
   bsta_gaussian_stats.h
   bsta_gaussian_updater.h
   bsta_mixture_functors.h
   bsta_bayes_functor.h
   bsta_adaptive_updater.hxx  bsta_adaptive_updater.h
   bsta_truth_updater.hxx     bsta_truth_updater.h
   bsta_parzen_updater.hxx    bsta_parzen_updater.h
   bsta_von_mises_updater.hxx bsta_von_mises_updater.h
   bsta_fit_weibull.cxx       bsta_fit_weibull.h
   bsta_mean_shift.hxx        bsta_mean_shift.h
   bsta_sample_set.hxx        bsta_sample_set.h
   bsta_beta_updater.hxx      bsta_beta_updater.h
   bsta_fit_gaussian.h
   bsta_sigma_normalizer.h    bsta_sigma_normalizer.cxx
   bsta_display_vrml.h
   bsta_mvnrand.h
   bsta_mog3_grey.h           bsta_mog3_grey.cxx
)

aux_source_directory(Templates bsta_algo_sources)

vxl_add_library(LIBRARY_NAME bsta_algo LIBRARY_SOURCES ${bsta_algo_sources} )
target_link_libraries(bsta_algo bsta ${VXL_LIB_PREFIX}vnl_algo ${VXL_LIB_PREFIX}vnl ${VXL_LIB_PREFIX}vbl)

if( BUILD_TESTING )
  add_subdirectory(tests)
endif()