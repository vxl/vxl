// This is mul/vil3d/tests/test_algo_convolve_1d.cxx
#include <testlib/testlib_test.h>
#include <vcl_iostream.h>
#include <vxl_config.h> // for vxl_byte
#include <vil3d/vil3d_image_view.h>
#include <vil3d/vil3d_switch_axes.h>
#include <vil3d/vil3d_print.h>
#include <vil3d/algo/vil3d_convolve_1d.h>


static void test_algo_convolve_1d()
{
  vcl_cout << "*********************************************\n"
           << " Testing vil3d_algo_convolve_1d\n"
           << "*********************************************\n";

  unsigned ni=5, nj=4, nk=3;

  vil3d_image_view<vxl_byte> src_im(ni,nj,nk,1);
  src_im.fill(0);
  src_im(2,2,2)= 64;
  float kernel[3]= {0.5, 0, -0.5};


  vil3d_image_view<vxl_sbyte> smoothed1_im, smoothed2_im, smoothed3_im, test_im(ni,nj,nk,1);

  vil3d_convolve_1d(src_im, smoothed1_im, kernel+1,-1,1,float(),
    vil_convolve_constant_extend, vil_convolve_zero_extend);

  vcl_cout << "Input image" << vcl_endl;
  vil3d_print_all(vcl_cout, src_im);
  vcl_cout << "Image after smoothing in i" << vcl_endl;
  vil3d_print_all(vcl_cout, smoothed1_im);
  
  test_im.fill(0);
  test_im(1,2,2) = 32;
  test_im(3,2,2) = -32;
  TEST("Correct output image after convolution",vil3d_image_view_deep_equality(test_im, smoothed1_im), true);
  
  vil3d_convolve_1d(vil3d_switch_axes_jki(smoothed1_im), smoothed2_im,
    kernel+1,-1,1,float(),
    vil_convolve_zero_extend, vil_convolve_zero_extend);
  smoothed2_im = vil3d_switch_axes_kij(smoothed2_im);
  
  vcl_cout << "Image after smoothing in i and j" << vcl_endl;
  vil3d_print_all(vcl_cout, smoothed2_im);

  test_im.fill(0);
  test_im(1,1,2) = 16;
  test_im(1,3,2) = -16;
  test_im(3,1,2) = -16;
  test_im(3,3,2) = 16;
  TEST("Correct output image after convolution",vil3d_image_view_deep_equality(test_im, smoothed2_im), true);
  
  vil3d_convolve_1d(vil3d_switch_axes_kij(smoothed2_im), smoothed3_im,
    kernel+1,-1,1,float(),
    vil_convolve_zero_extend, vil_convolve_zero_extend);
  smoothed3_im = vil3d_switch_axes_jki(smoothed3_im);
 
  vcl_cout << "Image after smoothing in i, j and k" << vcl_endl;
  vil3d_print_all(vcl_cout, smoothed3_im);
  
  test_im.fill(0);
  test_im(1,1,1) = 8;
  test_im(1,3,1) = -8;
  test_im(3,1,1) = -8;
  test_im(3,3,1) = 8;
  TEST("Correct output image after convolution",vil3d_image_view_deep_equality(test_im, smoothed3_im), true);

}


TESTMAIN(test_algo_convolve_1d);
