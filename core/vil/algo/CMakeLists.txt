# core/vil2/algo/CMakeLists.txt

SET( vil2_algo_sources
  vil2_sobel_1x3.cxx           vil2_sobel_1x3.h    vil2_sobel_1x3.txx
  vil2_sobel_3x3.cxx           vil2_sobel_3x3.h    vil2_sobel_3x3.txx
  vil2_gauss_filter.cxx        vil2_gauss_filter.h vil2_gauss_filter.txx
  vil2_gauss_reduce.cxx        vil2_gauss_reduce.h vil2_gauss_reduce.txx
  vil2_median.txx              vil2_median.h
  vil2_structuring_element.cxx vil2_structuring_element.h
  vil2_binary_dilate.cxx       vil2_binary_dilate.h
  vil2_binary_erode.cxx        vil2_binary_erode.h
  vil2_greyscale_dilate.txx    vil2_greyscale_dilate.h
  vil2_greyscale_erode.txx     vil2_greyscale_erode.h
                               vil2_greyscale_opening.h
                               vil2_greyscale_closing.h
                               vil2_binary_opening.h
                               vil2_binary_closing.h
                               vil2_convolve_1d.h
                               vil2_convolve_2d.h
                               vil2_correlate_1d.h
                               vil2_correlate_2d.h
                               vil2_exp_filter_1d.h
                               vil2_normalised_correlation_2d.h
                               vil2_exp_grad_filter_1d.h
  vil2_line_filter.txx         vil2_line_filter.h
  vil2_threshold.txx           vil2_threshold.h
                               vil2_grid_merge.h
                               vil2_find_4con_boundary.h
)


AUX_SOURCE_DIRECTORY(Templates vil2_algo_sources)

ADD_LIBRARY(vil2_algo ${vil2_algo_sources})

TARGET_LINK_LIBRARIES( vil2_algo vil2 vil vnl vcl )
