# mul/vil3d/algo/CMakeLists.txt

SET(vil3d_algo_sources
  vil3d_gauss_reduce.h
  vil3d_threshold.h           vil3d_threshold.txx
  vil3d_grad_1x3.h            vil3d_grad_1x3.cxx           vil3d_grad_1x3.txx
  vil3d_structuring_element.h vil3d_structuring_element.cxx
  vil3d_binary_dilate.h       vil3d_binary_dilate.cxx
  vil3d_binary_erode.h        vil3d_binary_erode.cxx
  vil3d_binary_opening.h
  vil3d_binary_closing.h
  vil3d_exp_filter.h
  vil3d_normalised_correlation_3d.h
  vil3d_convolve_1d.h
)

AUX_SOURCE_DIRECTORY(Templates vil3d_algo_sources)

ADD_LIBRARY(vil3d_algo ${vil3d_algo_sources})
TARGET_LINK_LIBRARIES( vil3d_algo vil3d vil_algo )
