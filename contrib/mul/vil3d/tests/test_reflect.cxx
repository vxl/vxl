// This is mul/vil3d/tests/test_reflect.cxx

#include <testlib/testlib_test.h>
#include <vcl_ctime.h>
#include <vcl_iostream.h>
#include <vxl_config.h>
#include <vil3d/vil3d_reflect.h>
#include <vil3d/vil3d_print.h>


//========================================================================
// Test vil3d_reflect() in i, j and k directions
//========================================================================
static void test_reflect_int()
{
  vcl_cout << "********************************** \n"
           << " Testing vil3d_reflect<vxl_int_32> \n"
           << "********************************** \n";

  const unsigned ni = 5;
  const unsigned nj = 5;
  const unsigned nk = 5;
  
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
  vcl_cout << "Original image:\n";
  vil3d_print_all(vcl_cout, image);
  vcl_cout << vcl_endl;

  vcl_cout << "Reflecting in i:\n";
  vil3d_print_all(vcl_cout, tempi);
  vcl_cout << vcl_endl;

  vcl_cout << "Reflecting in j:\n";
  vil3d_print_all(vcl_cout, tempj);
  vcl_cout << vcl_endl;

  vcl_cout << "Reflecting in k:\n";
  vil3d_print_all(vcl_cout, tempk);
  vcl_cout << vcl_endl;
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


//========================================================================
// Compare timing of vil3d_reflect() with actual transposition of image data
//========================================================================
static void time_reflect()
{
  vcl_cout << "********************************** \n"
           << " Timing vil3d_reflect<vxl_int_32> \n"
           << "********************************** \n";
  
  const unsigned ni = 256;
  const unsigned nj = 256;
  const unsigned nk = 256;
  
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
  
  vcl_clock_t beg_time1 = vcl_clock();
  //
  vil3d_image_view<vxl_int_32> temp1 = vil3d_reflect_k(image);
  //
  vcl_clock_t end_time1 = vcl_clock();
  double duration1 = (double)(end_time1 - beg_time1)/CLOCKS_PER_SEC;


  vcl_clock_t beg_time2 = vcl_clock();
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
  vcl_clock_t end_time2 = vcl_clock();
  double duration2 = (double)(end_time2 - beg_time2)/CLOCKS_PER_SEC;

  vcl_cout << "==============================================================\n";
  vcl_cout << "vil3d_reflect() duration: " << duration1      << " s. \n";
  vcl_cout << "transposition duration:   " << duration2      << " s. \n";
  vcl_cout << "==============================================================\n";
  vcl_cout << vcl_endl;    

}


//========================================================================
// Main testing function
//========================================================================
static void test_reflect()
{
  test_reflect_int();
  time_reflect();
}

TESTMAIN(test_reflect);
