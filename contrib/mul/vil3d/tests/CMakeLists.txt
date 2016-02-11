# This is mul/vil3d/tests/CMakeLists.txt

add_executable( vil3d_test_all
  test_driver.cxx

  test_file_format_read.cxx
  test_save_load_image.cxx
  test_gen_synthetic.cxx
  test_image_view.cxx
  test_image_resource.cxx
  test_math.cxx
  test_trilin_interp.cxx
  test_sample_profile_trilin.cxx
  test_switch_axes.cxx
  test_convert.cxx
  test_from_image_2d.cxx
  test_resample.cxx
  test_analyze_format.cxx
  test_reflect.cxx
  test_tricub_interp.cxx

  test_algo_gauss_reduce.cxx
  test_algo_threshold.cxx
  test_algo_structuring_element.cxx
  test_algo_binary_dilate.cxx
  test_algo_binary_erode.cxx
  test_algo_erode.cxx
  test_algo_exp_filter.cxx
  test_algo_grad_1x3.cxx
  test_algo_normalised_correlation_3d.cxx
  test_algo_convolve_1d.cxx
  test_algo_histogram.cxx
  test_algo_histogram_equalise.cxx
  test_algo_anisotropic_filter.cxx
  test_algo_smooth_121.cxx
  test_algo_corners.cxx
  test_algo_quad_distance_function.cxx
  test_algo_distance_transform.cxx
  test_algo_abs_shuffle_distance.cxx
  test_algo_make_distance_filter.cxx
  test_algo_exp_distance_transform.cxx
  test_algo_find_blobs.cxx
)

target_link_libraries( vil3d_test_all vil3d_algo vil3d ${VXL_LIB_PREFIX}vil ${VXL_LIB_PREFIX}vpl ${VXL_LIB_PREFIX}vul ${VXL_LIB_PREFIX}testlib ${VXL_LIB_PREFIX}vcl )

add_test( NAME vil3d_test_file_format_read COMMAND $<TARGET_FILE:vil3d_test_all> test_file_format_read ${CMAKE_CURRENT_SOURCE_DIR}/file_read_data )
add_test( NAME vil3d_test_save_load_image COMMAND $<TARGET_FILE:vil3d_test_all> test_save_load_image )
add_test( NAME vil3d_test_gen_synthetic COMMAND $<TARGET_FILE:vil3d_test_all> test_gen_synthetic)
add_test( NAME vil3d_test_image_view COMMAND $<TARGET_FILE:vil3d_test_all> test_image_view )
add_test( NAME vil3d_test_math COMMAND $<TARGET_FILE:vil3d_test_all> test_math )
add_test( NAME vil3d_test_trilin_interp COMMAND $<TARGET_FILE:vil3d_test_all> test_trilin_interp )
add_test( NAME vil3d_test_sample_profile_trilin COMMAND $<TARGET_FILE:vil3d_test_all> test_sample_profile_trilin )
add_test( NAME vil3d_test_switch_axes COMMAND $<TARGET_FILE:vil3d_test_all> test_switch_axes )
add_test( NAME vil3d_test_image_resource COMMAND $<TARGET_FILE:vil3d_test_all> test_image_resource )
add_test( NAME vil3d_test_convert COMMAND $<TARGET_FILE:vil3d_test_all> test_convert ${CMAKE_CURRENT_SOURCE_DIR}/file_read_data)
add_test( NAME vil3d_test_from_image_2d COMMAND $<TARGET_FILE:vil3d_test_all> test_from_image_2d )
add_test( NAME vil3d_test_resample COMMAND $<TARGET_FILE:vil3d_test_all> test_resample )
add_test( NAME vil3d_test_analyze_format COMMAND $<TARGET_FILE:vil3d_test_all> test_analyze_format ${CMAKE_CURRENT_SOURCE_DIR}/file_read_data/analyze)
add_test( NAME vil3d_test_reflect COMMAND $<TARGET_FILE:vil3d_test_all> test_reflect )
add_test( NAME vil3d_test_tricub_interp COMMAND $<TARGET_FILE:vil3d_test_all> test_tricub_interp )

add_test( NAME vil3d_test_algo_gauss_reduce COMMAND $<TARGET_FILE:vil3d_test_all>  test_algo_gauss_reduce )
add_test( NAME vil3d_test_algo_threshold COMMAND $<TARGET_FILE:vil3d_test_all>  test_algo_threshold )
add_test( NAME vil3d_test_algo_structuring_element COMMAND $<TARGET_FILE:vil3d_test_all>  test_algo_structuring_element )
add_test( NAME vil3d_test_algo_binary_dilate COMMAND $<TARGET_FILE:vil3d_test_all>  test_algo_binary_dilate )
add_test( NAME vil3d_test_algo_binary_erode COMMAND $<TARGET_FILE:vil3d_test_all>  test_algo_binary_erode )
add_test( NAME vil3d_test_algo_erode COMMAND $<TARGET_FILE:vil3d_test_all>  test_algo_erode )
add_test( NAME vil3d_test_algo_exp_filter COMMAND $<TARGET_FILE:vil3d_test_all>  test_algo_exp_filter )
add_test( NAME vil3d_test_algo_grad_1x3 COMMAND $<TARGET_FILE:vil3d_test_all>  test_algo_grad_1x3 )
add_test( NAME vil3d_test_algo_normalised_correlation_3d COMMAND $<TARGET_FILE:vil3d_test_all>  test_algo_normalised_correlation_3d )
add_test( NAME vil3d_test_algo_convolve_1d COMMAND $<TARGET_FILE:vil3d_test_all>  test_algo_convolve_1d )
add_test( NAME vil3d_test_algo_histogram COMMAND $<TARGET_FILE:vil3d_test_all>  test_algo_histogram )
add_test( NAME vil3d_test_algo_histogram_equalise COMMAND $<TARGET_FILE:vil3d_test_all>  test_algo_histogram_equalise )
add_test( NAME vil3d_test_algo_anisotropic_filter COMMAND $<TARGET_FILE:vil3d_test_all>  test_algo_anisotropic_filter )
add_test( NAME vil3d_test_algo_smooth_121 COMMAND $<TARGET_FILE:vil3d_test_all>  test_algo_smooth_121 )
add_test( NAME vil3d_test_algo_corners COMMAND $<TARGET_FILE:vil3d_test_all>  test_algo_corners )
add_test( NAME vil3d_test_algo_quad_distance_function COMMAND $<TARGET_FILE:vil3d_test_all>  test_algo_quad_distance_function )
add_test( NAME vil3d_test_algo_distance_transform COMMAND $<TARGET_FILE:vil3d_test_all>  test_algo_distance_transform )
add_test( NAME vil3d_test_algo_abs_shuffle_distance COMMAND $<TARGET_FILE:vil3d_test_all>  test_algo_abs_shuffle_distance )
add_test( NAME vil3d_test_algo_make_distance_filter COMMAND $<TARGET_FILE:vil3d_test_all>  test_algo_make_distance_filter )
add_test( NAME vil3d_test_algo_exp_distance_transform COMMAND $<TARGET_FILE:vil3d_test_all>  test_algo_exp_distance_transform )
add_test( NAME vil3d_test_algo_find_blobs COMMAND $<TARGET_FILE:vil3d_test_all>  test_algo_find_blobs )

add_executable( vil3d_test_include test_include.cxx )
target_link_libraries( vil3d_test_include vil3d_algo vil3d ${VXL_LIB_PREFIX}vgl )
add_executable( vil3d_test_template_include test_template_include.cxx )
target_link_libraries( vil3d_test_template_include vil3d_algo vil3d ${VXL_LIB_PREFIX}vgl )
