# mul/vimt/algo/CMakeLists.txt

SET( vimt_algo_sources
  vimt_convolve_2d.h
  vimt_find_peaks.h
  vimt_find_troughs.h
  vimt_normalised_correlation_2d.h
  # This file merely forces the creation of an actual library
  vimt_dummy.cxx
)

AUX_SOURCE_DIRECTORY(Templates vimt_algo_sources)

ADD_LIBRARY(vimt_algo ${vimt_algo_sources})

TARGET_LINK_LIBRARIES( vimt_algo vimt vcl )
