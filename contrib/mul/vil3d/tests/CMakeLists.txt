# This is mul/vil3d/tests/CMakeLists.txt

ADD_EXECUTABLE( vil3d_test_driver
  test_driver.cxx

  test_image_view.cxx
  test_trilin_interp.cxx
  test_sample_profile_trilin.cxx
  test_gauss_reduce.cxx
	test_switch_axes.cxx
  test_algo_threshold.cxx
  test_algo_structuring_element.cxx
  test_algo_binary_dilate.cxx
  test_algo_binary_erode.cxx
	test_algo_exp_filter.cxx
)
TARGET_LINK_LIBRARIES( vil3d_test_driver vil3d vil3d_algo vil2 vil2_algo vpl vul testlib vcl )

ADD_TEST( vil3d_test_image_view             vil3d_test_driver test_image_view )
ADD_TEST( vil3d_test_trilin_interp          vil3d_test_driver test_trilin_interp )
ADD_TEST( vil3d_test_sample_profile_trilin  vil3d_test_driver test_sample_profile_trilin )
ADD_TEST( vil3d_test_gauss_reduce           vil3d_test_driver test_gauss_reduce )
ADD_TEST( vil3d_test_switch_axes            vil3d_test_driver test_switch_axes )
ADD_TEST( vil3d_test_algo_threshold         vil3d_test_driver test_algo_threshold )
ADD_TEST( vil3d_test_algo_structuring_element         vil3d_test_driver test_algo_structuring_element )
ADD_TEST( vil3d_test_algo_binary_dilate     vil3d_test_driver test_algo_binary_dilate )
ADD_TEST( vil3d_test_algo_binary_erode      vil3d_test_driver test_algo_binary_erode )
ADD_TEST( vil3d_test_algo_exp_filter        vil3d_test_driver test_algo_exp_filter )
