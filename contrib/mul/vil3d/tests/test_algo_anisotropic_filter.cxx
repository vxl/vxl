// This is mul/vil3d/tests/test_algo_anisotropic_filter.cxx

#include <testlib/testlib_test.h>
#include <vcl_iostream.h>
#include <vil3d/algo/vil3d_anisotropic_filter.h>
#include <vil3d/vil3d_print.h>


// Test the anisotropic_gaussian_filter
static void test_anisotropic_gaussian_filter()
{
  TEST("Dummy test", true, true);

  vcl_cout << "***********************************************\n"
           << " Testing vil3d_algo_anisotropic_gaussian_filter\n"
           << "***********************************************\n";
  
  unsigned ni=9, nj=9, nk=9;
  
  vil3d_image_view<int> src_im(ni, nj, nk, 1);
  src_im.fill(0);
  src_im(4, 4, 4) = 100;

  vil3d_image_view<int> dst_im(ni, nj, nk, 1);
  dst_im.fill(0);
  
  vgl_vector_3d<double> vox(1.0, 1.0, 1.0);
  vgl_vector_3d<double> sd(1.0, 1.0, 1.0);
  vil3d_anisotropic_gaussian_filter(src_im, dst_im, vox, sd);

//  vcl_cout << "Source image: \n";
//  vil3d_print_all(vcl_cout, src_im);

//  vcl_cout << "\n\nDestination image: \n";
//  vil3d_print_all(vcl_cout, dst_im);
}


// Test all anisotropic filters
static void test_algo_anisotropic_filter()
{
  test_anisotropic_gaussian_filter();
}


// Main tetsing function
TESTMAIN(test_algo_anisotropic_filter);
