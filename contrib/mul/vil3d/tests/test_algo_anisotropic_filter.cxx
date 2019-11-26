// This is mul/vil3d/tests/test_algo_anisotropic_filter.cxx

#include <iostream>
#include "testlib/testlib_test.h"
#ifdef _MSC_VER
#  include "vcl_msvc_warnings.h"
#endif
#include "vgl/vgl_vector_3d.h"
#include <vil3d/algo/vil3d_anisotropic_filter.h>
#include <vil3d/vil3d_print.h>
#include <vil3d/vil3d_math.h>


// Test the anisotropic_gaussian_filter
static void test_anisotropic_gaussian_filter()
{
  std::cout << "************************************************\n"
           << " Testing vil3d_algo_anisotropic_gaussian_filter\n"
           << "************************************************\n";

  // Image dimensions
  const unsigned ni=9, nj=9, nk=9;

  // Create the source image
  vil3d_image_view<int> src_im(ni, nj, nk, 1);
  src_im.fill(0);
  src_im(4, 4, 4) = 1000;

  // Create the destination image
  vil3d_image_view<int> dst_im(ni, nj, nk, 1);
  dst_im.fill(0);

  // Create some workspace images
  vil3d_image_view<float> work1(ni, nj, nk);
  vil3d_image_view<float> work2(ni, nj, nk);
  vil3d_image_view<float> work3(ni, nj, nk);

  // Specify the smoothing widths
  vgl_vector_3d<double> sd(1.0, 1.0, 1.0);

  // Generate and apply the filters
  vil3d_anisotropic_gaussian_filter(src_im, dst_im, sd, work1, work2, work3);

#ifdef DEBUG
  std::cout << "Source image:\n";
  vil3d_print_all(std::cout, src_im);

  std::cout << "\n\nDestination image:\n";
  vil3d_print_all(std::cout, dst_im);
#endif // DEBUG

  double src_sum = 0;
  vil3d_math_sum(src_sum, src_im, 0);

  double dst_sum = 0;
  vil3d_math_sum(dst_sum, dst_im, 0);

  TEST_NEAR("Sum of destination image", dst_sum, 1002, 1e-9);
}


// Test all anisotropic filters
static void test_algo_anisotropic_filter()
{
  test_anisotropic_gaussian_filter();
}


// Main testing function
TESTMAIN(test_algo_anisotropic_filter);
