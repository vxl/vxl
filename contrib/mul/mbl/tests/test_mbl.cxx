// First define testmain

#include <vnl/vnl_test.h>
#undef TESTMAIN
#define TESTMAIN(x)

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

#undef TESTMAIN
#define TESTMAIN(x) int main() \
  { vnl_test_start(#x); x(); return vnl_test_summary(); }

void run_test_mbl()
{
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
  test_histogram();
}

TESTMAIN(run_test_mbl);
