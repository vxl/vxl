#include <testlib/testlib_register.h>

DECLARE( test_jarque_bera );
DECLARE( test_parse_block );
DECLARE( test_lda );
DECLARE( test_file_data_wrapper );
DECLARE( test_selected_data_wrapper );
DECLARE( test_priority_bounded_queue );
DECLARE( test_stochastic_data_collector );
DECLARE( test_k_means );
DECLARE( test_matrix_products );
DECLARE( test_matxvec );
DECLARE( test_stats_1d );
DECLARE( test_stats_nd );
DECLARE( test_sum_1d );
DECLARE( test_gamma );
DECLARE( test_index_sort );
DECLARE( test_lru_cache );
DECLARE( test_clamped_plate_spline_2d );
DECLARE( test_histogram );
DECLARE( test_data_wrapper_mixer );
DECLARE( test_combination );
DECLARE( test_ar_process );
DECLARE( test_random_n_from_m );
DECLARE( test_select_n_from_m );
DECLARE( test_thin_plate_spline_2d );
DECLARE( test_thin_plate_spline_3d );
DECLARE( test_thin_plate_spline_weights_3d );
DECLARE( test_read_props );
DECLARE( test_correspond_points );
DECLARE( test_rbf_network );


void
register_tests()
{
  REGISTER( test_jarque_bera );
  REGISTER( test_parse_block );
  REGISTER( test_lda );
  REGISTER( test_file_data_wrapper );
  REGISTER( test_selected_data_wrapper );
  REGISTER( test_priority_bounded_queue );
  REGISTER( test_stochastic_data_collector );
  REGISTER( test_k_means );
  REGISTER( test_matrix_products );
  REGISTER( test_matxvec );
  REGISTER( test_stats_1d );
  REGISTER( test_stats_nd );
  REGISTER( test_sum_1d );
  REGISTER( test_gamma );
  REGISTER( test_index_sort );
  REGISTER( test_lru_cache );
  REGISTER( test_clamped_plate_spline_2d );
  REGISTER( test_histogram );
  REGISTER( test_data_wrapper_mixer );
  REGISTER( test_combination );
  REGISTER( test_ar_process );
  REGISTER( test_random_n_from_m );
  REGISTER( test_select_n_from_m );
  REGISTER( test_thin_plate_spline_2d );
  REGISTER( test_thin_plate_spline_3d );
  REGISTER( test_thin_plate_spline_weights_3d );
  REGISTER( test_read_props );
  REGISTER( test_correspond_points );
  REGISTER( test_rbf_network );
}

DEFINE_MAIN;
