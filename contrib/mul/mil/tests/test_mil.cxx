// First define testmain

#include <testlib/testlib_test.h>
#undef TESTLIB_DEFINE_MAIN
#define TESTLIB_DEFINE_MAIN(x)

#include "test_normalise_image_2d.cxx"
#include "test_convert_vil.cxx"
#include "test_byte_image_2d_io.cxx"
#include "test_transform_2d.cxx"
#include "test_image_2d_of.cxx"
#include "test_bilin_interp_2d.cxx"
#include "test_sample_grid_2d.cxx"
#include "test_sample_profile_2d.cxx"
#include "test_gauss_reduce_2d.cxx"
#include "test_gaussian_pyramid_builder_2d.cxx"
#include "test_gaussian_pyramid_builder_2d_general.cxx"
#include "test_scale_pyramid_builder_2d.cxx"
#include "test_algo_exp_filter_1d.cxx"
#include "test_algo_line_filter.cxx"
#include "test_algo_grad_filter_2d.cxx"
#include "test_algo_gaussian_filter.cxx"

#undef TESTLIB_DEFINE_MAIN
#define TESTLIB_DEFINE_MAIN(x) int main() \
  { testlib_test_start(#x); x(); return testlib_test_summary(); }

void run_test_mil()
{
  
  test_gaussian_pyramid_builder_2d_general();

}


TESTLIB_DEFINE_MAIN(run_test_mil);
