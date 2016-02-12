# mul/mbl/tests/CMakeLists.txt

add_executable( mbl_test_all
  test_driver.cxx
  test_jarque_bera.cxx
  test_parse_block.cxx
  test_parse_sequence.cxx
  test_parse_tuple.cxx
  test_parse_int_list.cxx
  test_parse_colon_pairs_list.cxx
  test_lda.cxx
  test_file_data_wrapper.cxx
  test_selected_data_wrapper.cxx
  test_priority_bounded_queue.cxx
  test_stochastic_data_collector.cxx
  test_k_means.cxx
  test_matrix_products.cxx
  test_matxvec.cxx
  test_stats_1d.cxx
  test_sample_stats_1d.cxx
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
  test_text_file.cxx
  test_mask.cxx
)

target_link_libraries(mbl_test_all mbl ${VXL_LIB_PREFIX}vgl ${VXL_LIB_PREFIX}vnl_io ${VXL_LIB_PREFIX}vnl ${VXL_LIB_PREFIX}vbl ${VXL_LIB_PREFIX}vsl ${VXL_LIB_PREFIX}vul ${VXL_LIB_PREFIX}vpl ${VXL_LIB_PREFIX}testlib )

add_test( NAME mbl_test_jarque_bera COMMAND $<TARGET_FILE:mbl_test_all> test_jarque_bera )
add_test( NAME mbl_test_parse_block COMMAND $<TARGET_FILE:mbl_test_all> test_parse_block )
add_test( NAME mbl_test_parse_sequence COMMAND $<TARGET_FILE:mbl_test_all> test_parse_sequence )
add_test( NAME mbl_test_parse_tuple COMMAND $<TARGET_FILE:mbl_test_all> test_parse_tuple )
add_test( NAME mbl_test_parse_int_list COMMAND $<TARGET_FILE:mbl_test_all> test_parse_int_list )
add_test( NAME mbl_test_parse_colon_pairs_list COMMAND $<TARGET_FILE:mbl_test_all> test_parse_colon_pairs_list )
add_test( NAME mbl_test_lda COMMAND $<TARGET_FILE:mbl_test_all> test_lda )
add_test( NAME mbl_test_file_data_wrapper COMMAND $<TARGET_FILE:mbl_test_all> test_file_data_wrapper )
add_test( NAME mbl_test_selected_data_wrapper COMMAND $<TARGET_FILE:mbl_test_all> test_selected_data_wrapper )
add_test( NAME mbl_test_priority_bounded_queue COMMAND $<TARGET_FILE:mbl_test_all> test_priority_bounded_queue )
add_test( NAME mbl_test_stochastic_data_collector COMMAND $<TARGET_FILE:mbl_test_all> test_stochastic_data_collector )
add_test( NAME mbl_test_k_means COMMAND $<TARGET_FILE:mbl_test_all> test_k_means )
add_test( NAME mbl_test_matrix_products COMMAND $<TARGET_FILE:mbl_test_all> test_matrix_products )
add_test( NAME mbl_test_matxvec COMMAND $<TARGET_FILE:mbl_test_all> test_matxvec )
add_test( NAME mbl_test_stats_1d COMMAND $<TARGET_FILE:mbl_test_all> test_stats_1d )
add_test( NAME mbl_test_sample_stats_1d COMMAND $<TARGET_FILE:mbl_test_all> test_sample_stats_1d )
add_test( NAME mbl_test_stats_nd COMMAND $<TARGET_FILE:mbl_test_all> test_stats_nd )
add_test( NAME mbl_test_sum_1d COMMAND $<TARGET_FILE:mbl_test_all> test_sum_1d )
add_test( NAME mbl_test_gamma COMMAND $<TARGET_FILE:mbl_test_all> test_gamma )
add_test( NAME mbl_test_index_sort COMMAND $<TARGET_FILE:mbl_test_all> test_index_sort )
add_test( NAME mbl_test_lru_cache COMMAND $<TARGET_FILE:mbl_test_all> test_lru_cache )
add_test( NAME mbl_test_clamped_plate_spline_2d COMMAND $<TARGET_FILE:mbl_test_all> test_clamped_plate_spline_2d )
add_test( NAME mbl_test_histogram COMMAND $<TARGET_FILE:mbl_test_all> test_histogram )
add_test( NAME mbl_test_data_wrapper_mixer COMMAND $<TARGET_FILE:mbl_test_all> test_data_wrapper_mixer )
add_test( NAME mbl_test_combination COMMAND $<TARGET_FILE:mbl_test_all> test_combination )
add_test( NAME mbl_test_ar_process COMMAND $<TARGET_FILE:mbl_test_all> test_ar_process )
add_test( NAME mbl_test_random_n_from_m COMMAND $<TARGET_FILE:mbl_test_all> test_random_n_from_m )
add_test( NAME mbl_test_select_n_from_m COMMAND $<TARGET_FILE:mbl_test_all> test_select_n_from_m )
add_test( NAME mbl_test_thin_plate_spline_2d COMMAND $<TARGET_FILE:mbl_test_all> test_thin_plate_spline_2d )
add_test( NAME mbl_test_thin_plate_spline_3d COMMAND $<TARGET_FILE:mbl_test_all> test_thin_plate_spline_3d )
add_test( NAME mbl_test_thin_plate_spline_weights_3d COMMAND $<TARGET_FILE:mbl_test_all> test_thin_plate_spline_weights_3d )
add_test( NAME mbl_test_read_props COMMAND $<TARGET_FILE:mbl_test_all> test_read_props )
add_test( NAME mbl_test_read_multi_props COMMAND $<TARGET_FILE:mbl_test_all> test_read_multi_props )
add_test( NAME mbl_test_correspond_points COMMAND $<TARGET_FILE:mbl_test_all> test_correspond_points )
add_test( NAME mbl_test_rbf_network COMMAND $<TARGET_FILE:mbl_test_all> test_rbf_network )
add_test( NAME mbl_test_table COMMAND $<TARGET_FILE:mbl_test_all> test_table )
add_test( NAME mbl_test_cloneables_factory COMMAND $<TARGET_FILE:mbl_test_all> test_cloneables_factory )
add_test( NAME mbl_rvm_regression_builder COMMAND $<TARGET_FILE:mbl_test_all> test_rvm_regression_builder )
add_test( NAME mbl_test_test COMMAND $<TARGET_FILE:mbl_test_all> test_test )
add_test( NAME mbl_test_log COMMAND $<TARGET_FILE:mbl_test_all> test_log )
add_test( NAME mbl_test_stl COMMAND $<TARGET_FILE:mbl_test_all> test_stl )
add_test( NAME mbl_test_cloneable_ptr COMMAND $<TARGET_FILE:mbl_test_all> test_cloneable_ptr )
add_test( NAME mbl_test_exception COMMAND $<TARGET_FILE:mbl_test_all> test_exception )
add_test( NAME mbl_gram_schmidt COMMAND $<TARGET_FILE:mbl_test_all> test_gram_schmidt)
add_test( NAME mbl_test_linear_interpolator COMMAND $<TARGET_FILE:mbl_test_all> test_linear_interpolator)
add_test( NAME mbl_test_stepwise_regression COMMAND $<TARGET_FILE:mbl_test_all> test_stepwise_regression)
add_test( NAME mbl_test_clusters COMMAND $<TARGET_FILE:mbl_test_all> test_clusters)
add_test( NAME mbl_test_cluster_tree COMMAND $<TARGET_FILE:mbl_test_all> test_cluster_tree)
add_test( NAME mbl_test_dyn_prog COMMAND $<TARGET_FILE:mbl_test_all> test_dyn_prog)
add_test( NAME mbl_test_progress COMMAND $<TARGET_FILE:mbl_test_all> test_progress)
add_test( NAME mbl_test_text_file COMMAND $<TARGET_FILE:mbl_test_all> test_text_file)
add_test( NAME mbl_test_mask COMMAND $<TARGET_FILE:mbl_test_all> test_mask)

add_executable( mbl_test_include test_include.cxx )
target_link_libraries( mbl_test_include mbl )
add_executable( mbl_test_template_include test_template_include.cxx )
target_link_libraries( mbl_test_template_include mbl )
