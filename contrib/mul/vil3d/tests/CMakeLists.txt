# This is mul/vil3d/tests/CMakeLists.txt

ADD_EXECUTABLE( vil3d_test_all
  test_driver.cxx

  test_file_format_read.cxx
  test_image_view.cxx
  test_image_resource.cxx
  test_math.cxx
  test_trilin_interp.cxx
  test_sample_profile_trilin.cxx
  test_gauss_reduce.cxx
  test_switch_axes.cxx
  test_convert.cxx
  test_from_image_2d.cxx
  test_resample.cxx

  test_algo_threshold.cxx
  test_algo_structuring_element.cxx
  test_algo_binary_dilate.cxx
  test_algo_binary_erode.cxx
  test_algo_exp_filter.cxx
  test_algo_grad_1x3.cxx
  test_algo_normalised_correlation_3d.cxx
  test_algo_convolve_1d.cxx
  test_algo_histogram.cxx
  test_algo_histogram_equalise.cxx
  test_algo_anisotropic_filter.cxx
)

TARGET_LINK_LIBRARIES( vil3d_test_all vil3d_algo vil3d vil vil_algo vpl vul testlib vcl )

ADD_TEST( vil3d_test_file_format_read         vil3d_test_all test_file_format_read ${CMAKE_CURRENT_SOURCE_DIR}/file_read_data )
ADD_TEST( vil3d_test_image_view               vil3d_test_all test_image_view )
ADD_TEST( vil3d_test_math                     vil3d_test_all test_math )
ADD_TEST( vil3d_test_trilin_interp            vil3d_test_all test_trilin_interp )
ADD_TEST( vil3d_test_sample_profile_trilin    vil3d_test_all test_sample_profile_trilin )
ADD_TEST( vil3d_test_gauss_reduce             vil3d_test_all test_gauss_reduce )
ADD_TEST( vil3d_test_switch_axes              vil3d_test_all test_switch_axes )
ADD_TEST( vil3d_test_image_resource           vil3d_test_all test_image_resource )
ADD_TEST( vil3d_test_convert                  vil3d_test_all test_convert ${CMAKE_CURRENT_SOURCE_DIR}/file_read_data)
ADD_TEST( vil3d_test_from_image_2d            vil3d_test_all test_from_image_2d )
ADD_TEST( vil3d_test_resample                 vil3d_test_all test_resample )

ADD_TEST( vil3d_test_algo_threshold                   vil3d_test_all  test_algo_threshold )
ADD_TEST( vil3d_test_algo_structuring_element         vil3d_test_all  test_algo_structuring_element )
ADD_TEST( vil3d_test_algo_binary_dilate               vil3d_test_all  test_algo_binary_dilate )
ADD_TEST( vil3d_test_algo_binary_erode                vil3d_test_all  test_algo_binary_erode )
ADD_TEST( vil3d_test_algo_exp_filter                  vil3d_test_all  test_algo_exp_filter )
ADD_TEST( vil3d_test_algo_grad_1x3                    vil3d_test_all  test_algo_grad_1x3 )
ADD_TEST( vil3d_test_algo_normalised_correlation_3d   vil3d_test_all  test_algo_normalised_correlation_3d )
ADD_TEST( vil3d_test_algo_convolve_1d                 vil3d_test_all  test_algo_convolve_1d )
ADD_TEST( vil3d_test_algo_histogram                   vil3d_test_all  test_algo_histogram )
ADD_TEST( vil3d_test_algo_histogram_equalise          vil3d_test_all  test_algo_histogram_equalise )
ADD_TEST( vil3d_test_algo_anisotropic_filter          vil3d_test_all  test_algo_anisotropic_filter )

ADD_EXECUTABLE( vil3d_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( vil3d_test_include vil3d_algo vil3d )
