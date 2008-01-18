# This is brl/bsta/algo/CMakeLists.txt
# The Brown Statistics Algorithms Library

SET( bsta_algo_sources
   bsta_gaussian_stats.h
   bsta_gaussian_updater.h    bsta_gaussian_updater.cxx
   bsta_mixture_functors.h
   bsta_bayes_functor.h
   bsta_adaptive_updater.txx  bsta_adaptive_updater.h
   bsta_truth_updater.txx     bsta_truth_updater.h
)

AUX_SOURCE_DIRECTORY(Templates bsta_algo_sources)

ADD_LIBRARY( bsta_algo ${bsta_algo_sources} )

TARGET_LINK_LIBRARIES( bsta_algo bsta)

#IF( BUILD_TESTING )
#  SUBDIRS(tests)
#ENDIF( BUILD_TESTING )




