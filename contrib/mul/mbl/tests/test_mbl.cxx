// First define testmain

#include <testlib/testlib_test.h>
#undef TESTLIB_DEFINE_MAIN
#define TESTLIB_DEFINE_MAIN(x)

#include "test_selected_data_wrapper.cxx"
#include "test_stochastic_data_collector.cxx"
#include "test_histogram.cxx"
#include "test_k_means.cxx"
#include "test_mz_random.cxx"
#include "test_matrix_products.cxx"
#include "test_matxvec.cxx"
#include "test_stats_1d.cxx"
#include "test_sum_1d.cxx"
#include "test_priority_bounded_queue.cxx"
#include "test_gamma.cxx"
#include "test_index_sort.cxx"
#include "test_lru_cache.cxx"
#include "test_thin_plate_spline_2d.cxx"
#include "test_clamped_plate_spline_2d.cxx"
#include "test_data_wrapper_mixer.cxx"
#include "test_file_data_wrapper.cxx"
#include "test_combination.cxx"
#include "test_ar_process.cxx"
#include "test_random_n_from_m.cxx"
#include "test_select_n_from_m.cxx"

#undef TESTLIB_DEFINE_MAIN
#define TESTLIB_DEFINE_MAIN(x) int main() \
  { testlib_test_start(#x); x(); return testlib_test_summary(); }

void run_test_mbl()
{
  test_file_data_wrapper();
  test_selected_data_wrapper();
  test_priority_bounded_queue();
  test_stochastic_data_collector();
  test_k_means();
  test_mz_random();
  test_matrix_products();
  test_matxvec();
  test_stats_1d();
  test_sum_1d();
  test_gamma();
  test_index_sort();
  test_lru_cache();
  test_thin_plate_spline_2d();
  test_clamped_plate_spline_2d();
  test_histogram();
  test_data_wrapper_mixer();
  test_combination();
  test_ar_process();
  test_random_n_from_m();
  test_select_n_from_m();
}

TESTLIB_DEFINE_MAIN(run_test_mbl);
