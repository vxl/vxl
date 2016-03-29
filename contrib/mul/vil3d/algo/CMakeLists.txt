# mul/vil3d/algo/CMakeLists.txt

set(vil3d_algo_sources
  vil3d_gauss_reduce.h        vil3d_gauss_reduce.hxx
  vil3d_threshold.h           vil3d_threshold.hxx
  vil3d_grad_1x3.h            vil3d_grad_1x3.cxx           vil3d_grad_1x3.hxx
  vil3d_grad_3x3x3.h
  vil3d_structuring_element.h vil3d_structuring_element.cxx
  vil3d_binary_dilate.h       vil3d_binary_dilate.cxx
  vil3d_binary_erode.h        vil3d_binary_erode.cxx
  vil3d_erode.h
  vil3d_histogram_equalise.h  vil3d_histogram_equalise.cxx
  vil3d_histogram.h           vil3d_histogram.cxx
  vil3d_binary_opening.h
  vil3d_binary_closing.h
  vil3d_exp_filter.h
  vil3d_locally_z_normalise.h
  vil3d_normalised_correlation_3d.h
  vil3d_convolve_1d.h
  vil3d_distance_transform.h    vil3d_distance_transform.cxx
  vil3d_exp_distance_transform.h
  vil3d_fill_boundary.h         vil3d_fill_boundary.cxx
  vil3d_anisotropic_filter.h
  vil3d_max_product_filter.h
  vil3d_corners.h               vil3d_corners.hxx
  vil3d_find_peaks.h
  vil3d_quad_distance_function.h
  vil3d_smooth_121.h
  vil3d_abs_shuffle_distance.h  vil3d_abs_shuffle_distance.hxx
  vil3d_make_distance_filter.h  vil3d_make_distance_filter.cxx
  vil3d_make_edt_filter.h       vil3d_make_edt_filter.cxx
  vil3d_overlap.h               vil3d_overlap.cxx
  vil3d_find_blobs.h            vil3d_find_blobs.cxx
  vil3d_rank_filter.h
)

aux_source_directory(Templates vil3d_algo_sources)

vxl_add_library(LIBRARY_NAME vil3d_algo LIBRARY_SOURCES ${vil3d_algo_sources})
target_link_libraries( vil3d_algo vil3d ${VXL_LIB_PREFIX}vil_algo ${VXL_LIB_PREFIX}vil ${VXL_LIB_PREFIX}vnl ${VXL_LIB_PREFIX}vgl )

if( BUILD_TESTING )
  add_subdirectory(tests)
endif()
