// This is mul/vil3d/tests/test_reflect.cxx

#include <iostream>
#include <ctime>
#include <testlib/testlib_test.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vxl_config.h>
#include <vil3d/vil3d_reflect.h>
#include <vil3d/vil3d_print.h>


//========================================================================
// Test vil3d_reflect() in i, j and k directions
//========================================================================
static void test_reflect_int()
{
  std::cout << "***********************************\n"
           << " Testing vil3d_reflect<vxl_int_32>\n"
           << "***********************************\n";

  constexpr unsigned ni = 5;
  constexpr unsigned nj = 5;
  constexpr unsigned nk = 5;

  vil3d_image_view<vxl_int_32> image(ni, nj, nk);
  for (unsigned k=0; k<nk; ++k)
  {
    for (unsigned j=0; j<nj; ++j)
    {
      for (unsigned i=0; i<ni; ++i)
      {
        image(i,j,k) = 100*k + 10*j + i;
      }
    }
  }

  vil3d_image_view<vxl_int_32> tempi = vil3d_reflect_i(image);
  vil3d_image_view<vxl_int_32> tempj = vil3d_reflect_j(image);
  vil3d_image_view<vxl_int_32> tempk = vil3d_reflect_k(image);


//--- Additional output for Debug configurations -------------------------
#ifndef NDEBUG
  std::cout << "Original image:\n";
  vil3d_print_all(std::cout, image);
  std::cout << std::endl;

  std::cout << "Reflecting in i:\n";
  vil3d_print_all(std::cout, tempi);
  std::cout << std::endl;

  std::cout << "Reflecting in j:\n";
  vil3d_print_all(std::cout, tempj);
  std::cout << std::endl;

  std::cout << "Reflecting in k:\n";
  vil3d_print_all(std::cout, tempk);
  std::cout << std::endl;
#endif
//------------------------------------------------------------------------


  bool refl_i_ok = true;
  bool refl_j_ok = true;
  bool refl_k_ok = true;
  for (unsigned k=0; k<nk; ++k)
  {
    unsigned rk = nk-1-k;
    for (unsigned j=0; j<nj; ++j)
    {
      unsigned rj = nj-1-j;
      for (unsigned i=0; i<ni; ++i)
      {
        unsigned ri = ni-1-i;
        refl_i_ok = refl_i_ok && (tempi(i, j, k) == image(ri, j, k));
        refl_j_ok = refl_j_ok && (tempj(i, j, k) == image(i, rj, k));
        refl_k_ok = refl_k_ok && (tempk(i, j, k) == image(i, j, rk));
      }
    }
  }

  TEST("vil3d_reflect_i", refl_i_ok, true);
  TEST("vil3d_reflect_j", refl_j_ok, true);
  TEST("vil3d_reflect_k", refl_k_ok, true);
}

#if 0
//========================================================================
// Compare timing of vil3d_reflect() with actual transposition of image data
//========================================================================
static void time_reflect()
{
  std::cout << "**********************************\n"
           << " Timing vil3d_reflect<vxl_int_32>\n"
           << "**********************************\n";

  constexpr unsigned ni = 256;
  constexpr unsigned nj = 256;
  constexpr unsigned nk = 256;

  vil3d_image_view<vxl_int_32> image(ni, nj, nk);
  for (unsigned k=0; k<nk; ++k)
  {
    for (unsigned j=0; j<nj; ++j)
    {
      for (unsigned i=0; i<ni; ++i)
      {
        image(i,j,k) = 1000000*k + 1000*j + i;
      }
    }
  }

  std::clock_t beg_time1 = std::clock();
  //
  vil3d_image_view<vxl_int_32> temp1 = vil3d_reflect_k(image);
  //
  std::clock_t end_time1 = std::clock();
  double duration1 = (double)(end_time1 - beg_time1)/CLOCKS_PER_SEC;


  std::clock_t beg_time2 = std::clock();
  //
  vil3d_image_view<vxl_int_32> temp2(ni, nj, nk);
  for (unsigned k=0; k<nk; ++k)
  {
    unsigned rk = nk-1-k;
    for (unsigned j=0; j<nj; ++j)
    {
      for (unsigned i=0; i<ni; ++i)
      {
        temp2(i, j, k) = image(i, j, rk);
      }
    }
  }
  //
  std::clock_t end_time2 = std::clock();
  double duration2 = (double)(end_time2 - beg_time2)/CLOCKS_PER_SEC;

  std::cout << "==============================================================\n"
           << "vil3d_reflect() duration: " << duration1      << " s.\n"
           << "transposition duration:   " << duration2      << " s.\n"
           << "==============================================================\n\n";
}
#endif

//========================================================================
// Main testing function
//========================================================================
static void test_reflect()
{
  test_reflect_int();

  // No need to run this routinely
  // time_reflect();
  //
}

TESTMAIN(test_reflect);
