# This is core/vil2/tests/CMakeLists.txt

ADD_EXECUTABLE( vil2_test_all
  test_driver.cxx

  # basic things
  test_image_resource.cxx
  test_image_view.cxx
  test_memory_chunk.cxx

  # file format readers/writers
  test_file_format_read.cxx
  test_save_load_image.cxx

  # image operations
  test_deep_copy_3_plane.cxx
  test_image_view_maths.cxx
  test_convert.cxx

  # Sampling Operations
  test_bilin_interp.cxx
  test_sample_profile_bilin.cxx
  test_sample_grid_bilin.cxx
  test_resample_bilin.cxx
  test_bicub_interp.cxx
  test_sample_profile_bicub.cxx
  test_sample_grid_bicub.cxx
  test_resample_bicub.cxx

  # Conversion to/from vil1 images
  test_vil1.cxx

  # vil2/algo
  test_algo_gauss_filter.cxx
  test_algo_gauss_reduce.cxx
  test_algo_convolve_1d.cxx
  test_algo_convolve_2d.cxx
  test_algo_correlate_1d.cxx
  test_algo_correlate_2d.cxx
  test_algo_exp_filter_1d.cxx
  test_pixel_format.cxx
  test_binary_erode.cxx
  test_binary_dilate.cxx
  test_greyscale_dilate.cxx
  test_greyscale_erode.cxx
  test_median.cxx
  test_algo_exp_grad_filter_1d.cxx
  test_algo_line_filter.cxx
  test_algo_threshold.cxx
  test_algo_grid_merge.cxx
  test_algo_find_4con_boundary.cxx
  test_algo_fft.cxx
)

IF(CMAKE_COMPILER_IS_GNUCXX)
  SET_SOURCE_FILES_PROPERTIES(test_algo_convolve_1d.cxx COMPILE_FLAGS -O0)
ENDIF(CMAKE_COMPILER_IS_GNUCXX)

TARGET_LINK_LIBRARIES( vil2_test_all vil2_algo vil2 vpl vul testlib vcl )

# basic things
ADD_TEST( vil2_test_image_resource          vil2_test_all test_image_resource)
ADD_TEST( vil2_test_image_view              vil2_test_all test_image_view)
ADD_TEST( vil2_test_memory_chunk            vil2_test_all test_memory_chunk)

# file format readers/writers
ADD_TEST( vil2_test_file_format_read        vil2_test_all test_file_format_read ${VXLCORE_INCLUDE_DIR}/vil2/tests/file_read_data)
ADD_TEST( vil2_test_save_load_image         vil2_test_all test_save_load_image)

# image operations
ADD_TEST( vil2_test_deep_copy_3_plane       vil2_test_all test_deep_copy_3_plane )
ADD_TEST( vil2_test_image_view_maths        vil2_test_all test_image_view_maths)
ADD_TEST( vil2_test_convert                 vil2_test_all test_convert ${VXLCORE_INCLUDE_DIR}/vil2/tests/file_read_data)

# Sampling Operations
ADD_TEST( vil2_test_bilin_interp            vil2_test_all test_bilin_interp)
ADD_TEST( vil2_test_sample_profile_bilin    vil2_test_all test_sample_profile_bilin)
ADD_TEST( vil2_test_sample_grid_bilin       vil2_test_all test_sample_grid_bilin)
ADD_TEST( vil2_test_resample_bilin          vil2_test_all test_resample_bilin)
ADD_TEST( vil2_test_bicub_interp            vil2_test_all test_bicub_interp)
ADD_TEST( vil2_test_sample_profile_bicub    vil2_test_all test_sample_profile_bicub)
ADD_TEST( vil2_test_sample_grid_bicub       vil2_test_all test_sample_grid_bicub)
ADD_TEST( vil2_test_resample_bicub          vil2_test_all test_resample_bicub)

# Conversion to/from vil1 images
ADD_TEST( vil2_test_vil1                    vil2_test_all test_vil1 ${VXLCORE_INCLUDE_DIR}/vil/tests/file_read_data)

# vil2/algo
ADD_TEST( vil2_test_algo_gauss_filter       vil2_test_all test_algo_gauss_filter)
ADD_TEST( vil2_test_algo_gauss_reduce       vil2_test_all test_algo_gauss_reduce)
ADD_TEST( vil2_test_algo_convolve_1d        vil2_test_all test_algo_convolve_1d)
ADD_TEST( vil2_test_algo_convolve_2d        vil2_test_all test_algo_convolve_2d)
ADD_TEST( vil2_test_algo_correlate_1d       vil2_test_all test_algo_correlate_1d)
ADD_TEST( vil2_test_algo_correlate_2d       vil2_test_all test_algo_correlate_2d)
ADD_TEST( vil2_test_algo_exp_filter_1d      vil2_test_all test_algo_exp_filter_1d)
ADD_TEST( vil2_test_pixel_format            vil2_test_all test_pixel_format)
ADD_TEST( vil2_test_binary_erode            vil2_test_all test_binary_erode)
ADD_TEST( vil2_test_binary_dilate           vil2_test_all test_binary_dilate)
ADD_TEST( vil2_test_greyscale_dilate        vil2_test_all test_greyscale_dilate)
ADD_TEST( vil2_test_greyscale_erode         vil2_test_all test_greyscale_erode)
ADD_TEST( vil2_test_median                  vil2_test_all test_median)
ADD_TEST( vil2_test_algo_exp_grad_filter_1d vil2_test_all test_algo_exp_grad_filter_1d)
ADD_TEST( vil2_test_algo_line_filter        vil2_test_all test_algo_line_filter)
ADD_TEST( vil2_test_algo_threshold          vil2_test_all test_algo_threshold)
ADD_TEST( vil2_test_algo_grid_merge         vil2_test_all test_algo_grid_merge )
ADD_TEST( vil2_test_algo_find_4con_boundary vil2_test_all test_algo_find_4con_boundary )
ADD_TEST( vil2_test_algo_fft vil2_test_all test_algo_fft )

# To run timing tests
#ADD_EXECUTABLE( vil2_test_access_timings vil2_test_access_timings.cxx)
#TARGET_LINK_LIBRARIES( vil2_test_access_timings vil2 mbl vcl )
#ADD_TEST( vil2_test_access_timings vil2_test_access_timings )

#ADD_EXECUTABLE( vil2_test_access_timings_noplanes vil2_test_access_timings_noplanes.cxx)
#TARGET_LINK_LIBRARIES( vil2_test_access_timings_noplanes vil2 mbl vcl )
#ADD_TEST( vil2_test_access_timings_noplanes vil2_test_access_timings_noplanes )

#ADD_EXECUTABLE( vil2_vil_compare_access_timings vil2_vil_compare_access_timings.cxx)
#TARGET_LINK_LIBRARIES( vil2_vil_compare_access_timings vil2 mbl vcl )
#ADD_TEST( vil2_vil_compare_access_timings vil2_vil_compare_access_timings )

#ADD_EXECUTABLE( vil2_test_pair_access_timings vil2_test_pair_access_timings.cxx)
#TARGET_LINK_LIBRARIES( vil2_test_pair_access_timings vil2 mbl vcl )
#ADD_TEST( vil2_test_pair_access_timings vil2_test_pair_access_timings )

#ADD_EXECUTABLE( vil2_test_random_access_timings vil2_test_random_access_timings.cxx)
#TARGET_LINK_LIBRARIES( vil2_test_random_access_timings vil2 mbl vcl )
#ADD_TEST( vil2_test_random_access_timings vil2_test_random_access_timings )

#ADD_EXECUTABLE( vil2_mil_gauss_general_timings vil2_mil_gauss_general_timings.cxx)
#TARGET_LINK_LIBRARIES( vil2_mil_gauss_general_timings vil2_algo mil vpl vul mbl testlib vcl )
#ADD_TEST( vil2_mil_gauss_general_timings vil2_mil_gauss_general_timings )

#ADD_EXECUTABLE( vil2_vil_compare_function_timings vil2_vil_compare_function_timings.cxx)
#TARGET_LINK_LIBRARIES( vil2_vil_compare_function_timings vil2 mbl vcl )
#ADD_TEST( vil2_vil_compare_function_timings vil2_vil_compare_function_timings )

ADD_EXECUTABLE( vil2_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( vil2_test_include vil2_algo vil2 )
