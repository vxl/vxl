# This is core/vil2/tests/CMakeLists.txt

ADD_EXECUTABLE( vil2_test_driver
  test_driver.cxx

  test_vil1.cxx
  test_convert.cxx
  test_pixel_format.cxx
  test_save_load_image.cxx
  test_file_format_read.cxx
  test_image_view.cxx
  test_image_resource.cxx
  test_bilin_interp.cxx
  test_sample_profile_bilin.cxx
  test_sample_grid_bilin.cxx
  test_resample_bilin.cxx
  test_algo_gauss_reduce.cxx
  test_algo_convolve_1d.cxx
  test_algo_correlate_1d.cxx
  test_algo_convolve_2d.cxx
  test_algo_correlate_2d.cxx
  test_algo_exp_filter_1d.cxx
  test_algo_gauss_filter.cxx
  test_algo_exp_grad_filter_1d.cxx
  test_image_view_maths.cxx
  test_memory_chunk.cxx
  test_binary_dilate.cxx
  test_binary_erode.cxx
  test_greyscale_dilate.cxx
  test_greyscale_erode.cxx
  test_median.cxx
  test_algo_line_filter.cxx
  test_algo_threshold.cxx
  test_deep_copy_3_plane.cxx
)

IF(CMAKE_COMPILER_IS_GNUCXX)
  SET_SOURCE_FILES_PROPERTIES(test_algo_convolve_1d.cxx COMPILE_FLAGS -O0)
ENDIF(CMAKE_COMPILER_IS_GNUCXX)

TARGET_LINK_LIBRARIES( vil2_test_driver vil2_algo vil2 vpl vul testlib vcl )

ADD_TEST( vil2_test_vil1              vil2_test_driver test_vil1 ${VXLCORE_INCLUDE_DIR}/vil/tests/file_read_data)
ADD_TEST( vil2_test_convert           vil2_test_driver test_convert ${VXLCORE_INCLUDE_DIR}/vil/tests/file_read_data)
ADD_TEST( vil2_test_pixel_format      vil2_test_driver test_pixel_format)
ADD_TEST( vil2_test_file_format_read  vil2_test_driver test_file_format_read ${VXLCORE_INCLUDE_DIR}/vil/tests/file_read_data)
ADD_TEST( vil2_test_save_load_image   vil2_test_driver test_save_load_image)
ADD_TEST( vil2_test_image_view        vil2_test_driver test_image_view)
ADD_TEST( vil2_test_image_resource    vil2_test_driver test_image_resource)
ADD_TEST( vil2_test_bilin_interp      vil2_test_driver test_bilin_interp)
ADD_TEST( vil2_test_sample_profile_bilin    vil2_test_driver test_sample_profile_bilin)
ADD_TEST( vil2_test_sample_grid_bilin vil2_test_driver test_sample_grid_bilin)
ADD_TEST( vil2_test_algo_gauss_reduce vil2_test_driver test_algo_gauss_reduce)
ADD_TEST( vil2_test_algo_gauss_filter vil2_test_driver test_algo_gauss_filter)
ADD_TEST( vil2_test_algo_convolve_1d  vil2_test_driver test_algo_convolve_1d)
ADD_TEST( vil2_test_algo_correlate_1d vil2_test_driver test_algo_correlate_1d)
ADD_TEST( vil2_test_algo_convolve_2d  vil2_test_driver test_algo_convolve_2d)
ADD_TEST( vil2_test_algo_correlate_2d vil2_test_driver test_algo_correlate_2d)
ADD_TEST( vil2_test_algo_exp_filter_1d      vil2_test_driver test_algo_exp_filter_1d)
ADD_TEST( vil2_test_algo_exp_grad_filter_1d vil2_test_driver test_algo_exp_grad_filter_1d)
ADD_TEST( vil2_test_image_view_maths  vil2_test_driver test_image_view_maths)
ADD_TEST( vil2_memory_chunk           vil2_test_driver test_memory_chunk)
ADD_TEST( vil2_resample_bilin         vil2_test_driver test_resample_bilin)
ADD_TEST( vil2_test_binary_dilate     vil2_test_driver test_binary_dilate)
ADD_TEST( vil2_test_binary_erode      vil2_test_driver test_binary_erode)
ADD_TEST( vil2_test_greyscale_dilate  vil2_test_driver test_greyscale_dilate)
ADD_TEST( vil2_test_greyscale_erode   vil2_test_driver test_greyscale_erode)
ADD_TEST( vil2_test_median            vil2_test_driver test_median)
ADD_TEST( vil2_test_algo_line_filter  vil2_test_driver test_algo_line_filter)
ADD_TEST( vil2_test_algo_threshold    vil2_test_driver test_algo_threshold)
ADD_TEST( test_deep_copy_3_plane      vil2_test_driver test_deep_copy_3_plane )


# To run timing tests
#ADD_EXECUTABLE( vil2_test_access_timings vil2_test_access_timings.cxx)
#TARGET_LINK_LIBRARIES( vil2_test_access_timings vil2 mbl vcl )
# ADD_TEST( vil2_test_access_timings vil2_test_access_timings )

#ADD_EXECUTABLE( vil2_test_access_timings_noplanes vil2_test_access_timings_noplanes.cxx)
#TARGET_LINK_LIBRARIES( vil2_test_access_timings_noplanes vil2 mbl vcl )
# ADD_TEST( vil2_test_access_timings_noplanes vil2_test_access_timings_noplanes )

#ADD_EXECUTABLE( vil2_vil_compare_access_timings vil2_vil_compare_access_timings.cxx)
#TARGET_LINK_LIBRARIES( vil2_vil_compare_access_timings vil2 mbl vcl )
# ADD_TEST( vil2_vil_compare_access_timings vil2_vil_compare_access_timings )

#ADD_EXECUTABLE( vil2_test_pair_access_timings vil2_test_pair_access_timings.cxx)
#TARGET_LINK_LIBRARIES( vil2_test_pair_access_timings vil2 mbl vcl )
# ADD_TEST( vil2_test_pair_access_timings vil2_test_pair_access_timings )

#ADD_EXECUTABLE( vil2_test_random_access_timings vil2_test_random_access_timings.cxx)
#TARGET_LINK_LIBRARIES( vil2_test_random_access_timings vil2 mbl vcl )
# ADD_TEST( vil2_test_random_access_timings vil2_test_random_access_timings )

#ADD_EXECUTABLE( vil2_mil_gauss_general_timings vil2_mil_gauss_general_timings.cxx)
#TARGET_LINK_LIBRARIES( vil2_mil_gauss_general_timings vil2_algo mil vpl vul mbl testlib vcl )
# ADD_TEST( vil2_mil_gauss_general_timings vil2_mil_gauss_general_timings )

#ADD_EXECUTABLE( vil2_vil_compare_function_timings vil2_vil_compare_function_timings.cxx)
#TARGET_LINK_LIBRARIES( vil2_vil_compare_function_timings vil2 mbl vcl )
# ADD_TEST( vil2_vil_compare_function_timings vil2_vil_compare_function_timings )

