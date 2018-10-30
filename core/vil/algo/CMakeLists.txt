# core/vil/algo/CMakeLists.txt

project(vil_algo)


set( vil_algo_sources
  vil_sobel_1x3.cxx                vil_sobel_1x3.h    vil_sobel_1x3.hxx
  vil_sobel_3x3.cxx                vil_sobel_3x3.h    vil_sobel_3x3.hxx
  vil_gauss_filter.cxx             vil_gauss_filter.h vil_gauss_filter.hxx
  vil_gauss_reduce.cxx             vil_gauss_reduce.h vil_gauss_reduce.hxx
  vil_median.hxx                   vil_median.h
  vil_structuring_element.cxx      vil_structuring_element.h
  vil_binary_dilate.cxx            vil_binary_dilate.h
  vil_binary_erode.cxx             vil_binary_erode.h
  vil_greyscale_dilate.hxx         vil_greyscale_dilate.h
  vil_greyscale_erode.hxx          vil_greyscale_erode.h
                                   vil_greyscale_opening.h
                                   vil_greyscale_closing.h
                                   vil_binary_opening.h
                                   vil_binary_closing.h
                                   vil_convolve_1d.h
                                   vil_convolve_2d.h
                                   vil_correlate_1d.h
                                   vil_correlate_2d.h
                                   vil_dog_filter_5tap.h
                                   vil_dog_pyramid.h
                                   vil_exp_filter_1d.h
                                   vil_normalised_correlation_2d.h
                                   vil_exp_grad_filter_1d.h
                                   vil_exp_filter_2d.h
                                   vil_suppress_non_max.h
                                   vil_suppress_non_plateau.h
                                   vil_quad_distance_function.h
  vil_suppress_non_max_edges.hxx   vil_suppress_non_max_edges.h
  vil_line_filter.hxx              vil_line_filter.h
  vil_threshold.hxx                vil_threshold.h
                                   vil_grid_merge.h
                                   vil_find_4con_boundary.h
                                   vil_find_peaks.h
                                   vil_find_plateaus.h
  vil_fft.hxx                      vil_fft.h
  vil_histogram.cxx                vil_histogram.h
  vil_histogram_equalise.cxx       vil_histogram_equalise.h
  vil_blob.cxx                     vil_blob.h
  vil_distance_transform.cxx       vil_distance_transform.h
  vil_corners.cxx                  vil_corners.h
  vil_region_finder.hxx            vil_region_finder.h
  vil_cartesian_differential_invariants.hxx  vil_cartesian_differential_invariants.h
                                   vil_tile_images.h
  vil_orientations.cxx             vil_orientations.h
  vil_colour_space.cxx             vil_colour_space.h
  vil_abs_shuffle_distance.hxx     vil_abs_shuffle_distance.h
  vil_checker_board.hxx            vil_checker_board.h
                                   vil_flood_fill.h
)

aux_source_directory(Templates vil_algo_sources)

vxl_add_library(LIBRARY_NAME ${VXL_LIB_PREFIX}vil_algo LIBRARY_SOURCES ${vil_algo_sources})

target_link_libraries( ${VXL_LIB_PREFIX}vil_algo ${VXL_LIB_PREFIX}vil ${VXL_LIB_PREFIX}vnl_algo ${VXL_LIB_PREFIX}vnl ${VXL_LIB_PREFIX}vcl )

if( VXL_BUILD_EXAMPLES AND VXL_VIL_INCLUDE_IMAGE_IO)
  add_subdirectory(examples)
endif()
if( BUILD_TESTING AND VXL_VIL_INCLUDE_IMAGE_IO)
  add_subdirectory(tests)
endif()

if (VXL_BUILD_OBJECT_LIBRARIES)
  add_library(vil_algo-obj OBJECT ${vil_algo_sources})
endif()
