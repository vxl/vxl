# mul/mil/algo/CMakeLists.txt

SET(mil_algo_sources
  mil_algo_exp_filter_1d.cxx     mil_algo_exp_filter_1d.h
  mil_algo_grad_3x3.cxx          mil_algo_grad_3x3.h
  mil_algo_grad_1x3.cxx          mil_algo_grad_1x3.h

  mil_algo_exp_filter_2d.txx     mil_algo_exp_filter_2d.h
  mil_algo_line_filter.txx       mil_algo_line_filter.h
  mil_algo_grad_filter_2d.txx    mil_algo_grad_filter_2d.h
  mil_algo_gaussian_filter.txx   mil_algo_gaussian_filter.h
  mil_algo_brighten_image_2d.txx mil_algo_brighten_image_2d.h
  mil_algo_convolve_1d.h
)
AUX_SOURCE_DIRECTORY(Templates mil_algo_sources)

ADD_LIBRARY(mil_algo ${mil_algo_sources})
TARGET_LINK_LIBRARIES(mil_algo mil vnl)
