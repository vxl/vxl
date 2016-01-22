# This is mul/vil3d/tests/CMakeLists.txt

ADD_EXECUTABLE( vil3d_test_all
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

TARGET_LINK_LIBRARIES( vil3d_test_all vil3d_algo vil3d vil vpl vul testlib vcl )

ADD_TEST( vil3d_test_file_format_read         ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/vil3d_test_all test_file_format_read ${CMAKE_CURRENT_SOURCE_DIR}/file_read_data )
ADD_TEST( vil3d_test_save_load_image          ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/vil3d_test_all test_save_load_image )
ADD_TEST( vil3d_test_gen_synthetic            ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/vil3d_test_all test_gen_synthetic)
ADD_TEST( vil3d_test_image_view               ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/vil3d_test_all test_image_view )
ADD_TEST( vil3d_test_math                     ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/vil3d_test_all test_math )
ADD_TEST( vil3d_test_trilin_interp            ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/vil3d_test_all test_trilin_interp )
ADD_TEST( vil3d_test_sample_profile_trilin    ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/vil3d_test_all test_sample_profile_trilin )
ADD_TEST( vil3d_test_switch_axes              ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/vil3d_test_all test_switch_axes )
ADD_TEST( vil3d_test_image_resource           ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/vil3d_test_all test_image_resource )
ADD_TEST( vil3d_test_convert                  ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/vil3d_test_all test_convert ${CMAKE_CURRENT_SOURCE_DIR}/file_read_data)
ADD_TEST( vil3d_test_from_image_2d            ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/vil3d_test_all test_from_image_2d )
ADD_TEST( vil3d_test_resample                 ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/vil3d_test_all test_resample )
ADD_TEST( vil3d_test_analyze_format           ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/vil3d_test_all test_analyze_format ${CMAKE_CURRENT_SOURCE_DIR}/file_read_data/analyze)
ADD_TEST( vil3d_test_reflect                  ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/vil3d_test_all test_reflect )
ADD_TEST( vil3d_test_tricub_interp            ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/vil3d_test_all test_tricub_interp )

ADD_TEST( vil3d_test_algo_gauss_reduce                ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/vil3d_test_all  test_algo_gauss_reduce )
ADD_TEST( vil3d_test_algo_threshold                   ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/vil3d_test_all  test_algo_threshold )
ADD_TEST( vil3d_test_algo_structuring_element         ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/vil3d_test_all  test_algo_structuring_element )
ADD_TEST( vil3d_test_algo_binary_dilate               ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/vil3d_test_all  test_algo_binary_dilate )
ADD_TEST( vil3d_test_algo_binary_erode                ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/vil3d_test_all  test_algo_binary_erode )
ADD_TEST( vil3d_test_algo_erode                       ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/vil3d_test_all  test_algo_erode )
ADD_TEST( vil3d_test_algo_exp_filter                  ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/vil3d_test_all  test_algo_exp_filter )
ADD_TEST( vil3d_test_algo_grad_1x3                    ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/vil3d_test_all  test_algo_grad_1x3 )
ADD_TEST( vil3d_test_algo_normalised_correlation_3d   ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/vil3d_test_all  test_algo_normalised_correlation_3d )
ADD_TEST( vil3d_test_algo_convolve_1d                 ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/vil3d_test_all  test_algo_convolve_1d )
ADD_TEST( vil3d_test_algo_histogram                   ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/vil3d_test_all  test_algo_histogram )
ADD_TEST( vil3d_test_algo_histogram_equalise          ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/vil3d_test_all  test_algo_histogram_equalise )
ADD_TEST( vil3d_test_algo_anisotropic_filter          ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/vil3d_test_all  test_algo_anisotropic_filter )
ADD_TEST( vil3d_test_algo_smooth_121                  ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/vil3d_test_all  test_algo_smooth_121 )
ADD_TEST( vil3d_test_algo_corners                     ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/vil3d_test_all  test_algo_corners )
ADD_TEST( vil3d_test_algo_quad_distance_function      ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/vil3d_test_all  test_algo_quad_distance_function )
ADD_TEST( vil3d_test_algo_distance_transform          ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/vil3d_test_all  test_algo_distance_transform )
ADD_TEST( vil3d_test_algo_abs_shuffle_distance        ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/vil3d_test_all  test_algo_abs_shuffle_distance )
ADD_TEST( vil3d_test_algo_make_distance_filter        ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/vil3d_test_all  test_algo_make_distance_filter )
ADD_TEST( vil3d_test_algo_exp_distance_transform      ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/vil3d_test_all  test_algo_exp_distance_transform )
ADD_TEST( vil3d_test_algo_find_blobs                  ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/vil3d_test_all  test_algo_find_blobs )

ADD_EXECUTABLE( vil3d_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( vil3d_test_include vil3d_algo vil3d vgl )
ADD_EXECUTABLE( vil3d_test_template_include test_template_include.cxx )
TARGET_LINK_LIBRARIES( vil3d_test_template_include vil3d_algo vil3d vgl )
