# mul/mbl/tests/CMakeLists.txt


ADD_EXECUTABLE( mbl_test_all
  test_driver.cxx
  test_jarque_bera.cxx
  test_parse_block.cxx
  test_parse_sequence.cxx
  test_parse_tuple.cxx
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
  test_read_multi_props.cxx
  test_correspond_points.cxx
  test_rbf_network.cxx
  test_table.cxx
  test_cloneables_factory.cxx
  test_rvm_regression_builder.cxx
  test_test.cxx
  test_log.cxx
  test_stl.cxx
  test_cloneable_ptr.cxx
  test_exception.cxx
  test_gram_schmidt.cxx
  test_linear_interpolator.cxx
  test_stepwise_regression.cxx
  test_clusters.cxx
  test_cluster_tree.cxx
  test_dyn_prog.cxx
  test_progress.cxx
  test_vector_txt_files.cxx
)

TARGET_LINK_LIBRARIES(mbl_test_all mbl vgl vnl_io vnl vbl vsl vul vpl testlib )


ADD_TEST( mbl_test_jarque_bera ${EXECUTABLE_OUTPUT_PATH}/mbl_test_all test_jarque_bera )
ADD_TEST( mbl_test_parse_block ${EXECUTABLE_OUTPUT_PATH}/mbl_test_all test_parse_block )
ADD_TEST( mbl_test_parse_sequence ${EXECUTABLE_OUTPUT_PATH}/mbl_test_all test_parse_sequence )
ADD_TEST( mbl_test_parse_tuple ${EXECUTABLE_OUTPUT_PATH}/mbl_test_all test_parse_tuple )
ADD_TEST( mbl_test_lda ${EXECUTABLE_OUTPUT_PATH}/mbl_test_all test_lda )
ADD_TEST( mbl_test_file_data_wrapper ${EXECUTABLE_OUTPUT_PATH}/mbl_test_all test_file_data_wrapper )
ADD_TEST( mbl_test_selected_data_wrapper ${EXECUTABLE_OUTPUT_PATH}/mbl_test_all test_selected_data_wrapper )
ADD_TEST( mbl_test_priority_bounded_queue ${EXECUTABLE_OUTPUT_PATH}/mbl_test_all test_priority_bounded_queue )
ADD_TEST( mbl_test_stochastic_data_collector ${EXECUTABLE_OUTPUT_PATH}/mbl_test_all test_stochastic_data_collector )
ADD_TEST( mbl_test_k_means ${EXECUTABLE_OUTPUT_PATH}/mbl_test_all test_k_means )
ADD_TEST( mbl_test_matrix_products ${EXECUTABLE_OUTPUT_PATH}/mbl_test_all test_matrix_products )
ADD_TEST( mbl_test_matxvec ${EXECUTABLE_OUTPUT_PATH}/mbl_test_all test_matxvec )
ADD_TEST( mbl_test_stats_1d ${EXECUTABLE_OUTPUT_PATH}/mbl_test_all test_stats_1d )
ADD_TEST( mbl_test_stats_nd ${EXECUTABLE_OUTPUT_PATH}/mbl_test_all test_stats_nd )
ADD_TEST( mbl_test_sum_1d ${EXECUTABLE_OUTPUT_PATH}/mbl_test_all test_sum_1d )
ADD_TEST( mbl_test_gamma ${EXECUTABLE_OUTPUT_PATH}/mbl_test_all test_gamma )
ADD_TEST( mbl_test_index_sort ${EXECUTABLE_OUTPUT_PATH}/mbl_test_all test_index_sort )
ADD_TEST( mbl_test_lru_cache ${EXECUTABLE_OUTPUT_PATH}/mbl_test_all test_lru_cache )
ADD_TEST( mbl_test_clamped_plate_spline_2d ${EXECUTABLE_OUTPUT_PATH}/mbl_test_all test_clamped_plate_spline_2d )
ADD_TEST( mbl_test_histogram ${EXECUTABLE_OUTPUT_PATH}/mbl_test_all test_histogram )
ADD_TEST( mbl_test_data_wrapper_mixer ${EXECUTABLE_OUTPUT_PATH}/mbl_test_all test_data_wrapper_mixer )
ADD_TEST( mbl_test_combination ${EXECUTABLE_OUTPUT_PATH}/mbl_test_all test_combination )
ADD_TEST( mbl_test_ar_process ${EXECUTABLE_OUTPUT_PATH}/mbl_test_all test_ar_process )
ADD_TEST( mbl_test_random_n_from_m ${EXECUTABLE_OUTPUT_PATH}/mbl_test_all test_random_n_from_m )
ADD_TEST( mbl_test_select_n_from_m ${EXECUTABLE_OUTPUT_PATH}/mbl_test_all test_select_n_from_m )
ADD_TEST( mbl_test_thin_plate_spline_2d ${EXECUTABLE_OUTPUT_PATH}/mbl_test_all test_thin_plate_spline_2d )
ADD_TEST( mbl_test_thin_plate_spline_3d ${EXECUTABLE_OUTPUT_PATH}/mbl_test_all test_thin_plate_spline_3d )
ADD_TEST( mbl_test_thin_plate_spline_weights_3d ${EXECUTABLE_OUTPUT_PATH}/mbl_test_all test_thin_plate_spline_weights_3d )
ADD_TEST( mbl_test_read_props ${EXECUTABLE_OUTPUT_PATH}/mbl_test_all test_read_props )
ADD_TEST( mbl_test_read_multi_props ${EXECUTABLE_OUTPUT_PATH}/mbl_test_all test_read_multi_props )
ADD_TEST( mbl_test_correspond_points ${EXECUTABLE_OUTPUT_PATH}/mbl_test_all test_correspond_points )
ADD_TEST( mbl_test_rbf_network ${EXECUTABLE_OUTPUT_PATH}/mbl_test_all test_rbf_network )
ADD_TEST( mbl_test_table ${EXECUTABLE_OUTPUT_PATH}/mbl_test_all test_table )
ADD_TEST( mbl_test_cloneables_factory ${EXECUTABLE_OUTPUT_PATH}/mbl_test_all test_cloneables_factory )
ADD_TEST( mbl_rvm_regression_builder ${EXECUTABLE_OUTPUT_PATH}/mbl_test_all test_rvm_regression_builder )
ADD_TEST( mbl_test_test ${EXECUTABLE_OUTPUT_PATH}/mbl_test_all test_test )
ADD_TEST( mbl_test_log ${EXECUTABLE_OUTPUT_PATH}/mbl_test_all test_log )
ADD_TEST( mbl_test_stl ${EXECUTABLE_OUTPUT_PATH}/mbl_test_all test_stl )
ADD_TEST( mbl_test_cloneable_ptr ${EXECUTABLE_OUTPUT_PATH}/mbl_test_all test_cloneable_ptr )
ADD_TEST( mbl_test_exception ${EXECUTABLE_OUTPUT_PATH}/mbl_test_all test_exception )
ADD_TEST (mbl_gram_schmidt ${EXECUTABLE_OUTPUT_PATH}/mbl_test_all test_gram_schmidt)
ADD_TEST (mbl_test_linear_interpolator ${EXECUTABLE_OUTPUT_PATH}/mbl_test_all test_linear_interpolator)
ADD_TEST (mbl_test_stepwise_regression ${EXECUTABLE_OUTPUT_PATH}/mbl_test_all test_stepwise_regression)
ADD_TEST (mbl_test_clusters ${EXECUTABLE_OUTPUT_PATH}/mbl_test_all test_clusters)
ADD_TEST (mbl_test_cluster_tree ${EXECUTABLE_OUTPUT_PATH}/mbl_test_all test_cluster_tree)
ADD_TEST (mbl_test_dyn_prog ${EXECUTABLE_OUTPUT_PATH}/mbl_test_all test_dyn_prog)
ADD_TEST (mbl_test_progress ${EXECUTABLE_OUTPUT_PATH}/mbl_test_all test_progress)
ADD_TEST (mbl_vector_txt_files ${EXECUTABLE_OUTPUT_PATH}/mbl_test_all test_vector_txt_files)


ADD_EXECUTABLE( mbl_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( mbl_test_include mbl )
