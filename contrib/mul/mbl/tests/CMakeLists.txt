# mul/mbl/tests/CMakeLists.txt


ADD_EXECUTABLE( mbl_test_all
  test_driver.cxx
  test_jarque_bera.cxx
  test_parse_block.cxx
  test_lda.cxx
  test_file_data_wrapper.cxx
  test_selected_data_wrapper.cxx
  test_priority_bounded_queue.cxx
  test_stochastic_data_collector.cxx
  test_k_means.cxx
  test_matrix_products.cxx
  test_matxvec.cxx
  test_stats_1d.cxx
  test_stats_nd.cxx
  test_sum_1d.cxx
  test_gamma.cxx
  test_index_sort.cxx
  test_lru_cache.cxx
  test_clamped_plate_spline_2d.cxx
  test_histogram.cxx
  test_data_wrapper_mixer.cxx
  test_combination.cxx
  test_ar_process.cxx
  test_random_n_from_m.cxx
  test_select_n_from_m.cxx
  test_thin_plate_spline_2d.cxx
  test_thin_plate_spline_3d.cxx
  test_thin_plate_spline_weights_3d.cxx
  test_read_props.cxx
  test_correspond_points.cxx
  test_rbf_network.cxx
  test_table.cxx
  test_cloneables_factory.cxx
)

TARGET_LINK_LIBRARIES(mbl_test_all mbl vbl vul vpl testlib )


ADD_TEST( mbl_test_jarque_bera mbl_test_all test_jarque_bera )
ADD_TEST( mbl_test_parse_block mbl_test_all test_parse_block )
ADD_TEST( mbl_test_lda mbl_test_all test_lda )
ADD_TEST( mbl_test_file_data_wrapper mbl_test_all test_file_data_wrapper )
ADD_TEST( mbl_test_selected_data_wrapper mbl_test_all test_selected_data_wrapper )
ADD_TEST( mbl_test_priority_bounded_queue mbl_test_all test_priority_bounded_queue )
ADD_TEST( mbl_test_stochastic_data_collector mbl_test_all test_stochastic_data_collector )
ADD_TEST( mbl_test_k_means mbl_test_all test_k_means )
ADD_TEST( mbl_test_matrix_products mbl_test_all test_matrix_products )
ADD_TEST( mbl_test_matxvec mbl_test_all test_matxvec )
ADD_TEST( mbl_test_stats_1d mbl_test_all test_stats_1d )
ADD_TEST( mbl_test_stats_nd mbl_test_all test_stats_nd )
ADD_TEST( mbl_test_sum_1d mbl_test_all test_sum_1d )
ADD_TEST( mbl_test_gamma mbl_test_all test_gamma )
ADD_TEST( mbl_test_index_sort mbl_test_all test_index_sort )
ADD_TEST( mbl_test_lru_cache mbl_test_all test_lru_cache )
ADD_TEST( mbl_test_clamped_plate_spline_2d mbl_test_all test_clamped_plate_spline_2d )
ADD_TEST( mbl_test_histogram mbl_test_all test_histogram )
ADD_TEST( mbl_test_data_wrapper_mixer mbl_test_all test_data_wrapper_mixer )
ADD_TEST( mbl_test_combination mbl_test_all test_combination )
ADD_TEST( mbl_test_ar_process mbl_test_all test_ar_process )
ADD_TEST( mbl_test_random_n_from_m mbl_test_all test_random_n_from_m )
ADD_TEST( mbl_test_select_n_from_m mbl_test_all test_select_n_from_m )
ADD_TEST( mbl_test_thin_plate_spline_2d mbl_test_all test_thin_plate_spline_2d )
ADD_TEST( mbl_test_thin_plate_spline_3d mbl_test_all test_thin_plate_spline_3d )
ADD_TEST( mbl_test_thin_plate_spline_weights_3d mbl_test_all test_thin_plate_spline_weights_3d )
ADD_TEST( mbl_test_read_props mbl_test_all test_read_props )
ADD_TEST( mbl_test_correspond_points mbl_test_all test_correspond_points )
ADD_TEST( mbl_test_rbf_network mbl_test_all test_rbf_network )
ADD_TEST( mbl_test_table mbl_test_all test_table )
ADD_TEST( mbl_test_cloneables_factory mbl_test_all test_cloneables_factory )


ADD_EXECUTABLE( mbl_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( mbl_test_include mbl )
