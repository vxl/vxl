// First define testmain

#include <vnl/vnl_test.h>
#undef TESTMAIN
#define TESTMAIN(x)

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

#undef TESTMAIN
#define TESTMAIN(x) int main() \
  { vnl_test_start(#x); x(); return vnl_test_summary(); }

void run_test_mil()
{
  
  test_normalise_image_2d();
  test_convert_vil();
  test_byte_image_2d_io();
  test_sample_profile_2d();
  test_sample_grid_2d();
  test_transform_2d();
  test_image_2d_of();
  test_bilin_interp_2d();
  test_gauss_reduce_2d();
  test_gaussian_pyramid_builder_2d();
  test_gaussian_pyramid_builder_2d_general();
  test_algo_exp_filter_1d();
  test_algo_line_filter();
  test_algo_grad_filter_2d();
  test_algo_gaussian_filter();
  test_scale_pyramid_builder_2d();

}


TESTMAIN(run_test_mil);
