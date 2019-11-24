// This is core/vil/algo/tests/test_algo_2nd_deriv.cxx
#include <iostream>
#include "testlib/testlib_test.h"
//:
// \file
// \author Peter Vanroose
// \date   26 Aug. 2004
#include <vcl_compiler.h>
#include "vxl_config.h" // for vxl_byte
#include <vil/algo/vil_2nd_deriv_3x3.h>

static void test_algo_2nd_deriv_3x3()
{
  std::cout << "*******************\n"
           << " Testing vil_2nd_deriv_3x3\n"
           << "*******************\n";

  const unsigned int ni=5, nj=6, i=2, j=3;
  vil_image_view<vxl_byte> image_d2i; image_d2i.set_size(ni,nj);
  vil_image_view<vxl_byte> image_d2j; image_d2j.set_size(ni,nj);
  vil_image_view<vxl_byte> image_d2I_didj; image_d2I_didj.set_size(ni,nj);
  image_d2i.fill(10); image_d2j.fill(10); image_d2I_didj.fill(10);
  vxl_byte v = 11, m = 9;
  image_d2i(i+1,j-1)=v;  image_d2i(i-1,j-1)=v;
  image_d2i(i+1,j)  =v;  image_d2i(i-1,j)  =v;
  image_d2i(i+1,j+1)=v;  image_d2i(i-1,j+1)=v;

  image_d2j(i-1,j+1)=v;  image_d2j(i-1,j-1)=v;
  image_d2j(i,  j+1)=v;  image_d2j(i,  j-1)=v;
  image_d2j(i+1,j+1)=v;  image_d2j(i+1,j-1)=v;

  image_d2I_didj(i-1,j+1)=v;  image_d2I_didj(i-1,j-1)=m;
  image_d2I_didj(i+1,j+1)=m;  image_d2I_didj(i+1,j-1)=v;



  vil_image_view<double> d2Idi, d2Idj, d2I_didj;
  vil_2nd_deriv_3x3(image_d2i, d2Idi, d2Idj, d2I_didj);
  TEST("vil_2nd_deriv_3x3: d2Idi2", d2Idi(i,j), 1.0);

  vil_2nd_deriv_3x3(image_d2j, d2Idi, d2Idj, d2I_didj);
  TEST("vil_2nd_deriv_3x3: d2Idj2", d2Idj(i,j), 1.0);

  vil_2nd_deriv_3x3(image_d2I_didj, d2Idi, d2Idj, d2I_didj);
  TEST("vil_2nd_deriv_3x3: d2I_didj", d2I_didj(i, j), 1.0);

  vil_image_view<double> multi_plane;
  vil_2nd_deriv_3x3(image_d2i, multi_plane);
  TEST("vil_2nd_deriv_3x3: multiplane d2Idi2", multi_plane(i, j, 0), 1.0);

  vil_2nd_deriv_3x3(image_d2j, multi_plane);
  TEST("vil_2nd_deriv_3x3: multiplane d2Idj2", multi_plane(i, j, 1), 1.0);

  vil_2nd_deriv_3x3(image_d2I_didj, multi_plane);
  TEST("vil_2nd_deriv_3x3: multiplane d2I_didj", multi_plane(i, j, 2), 1.0);
}

TESTMAIN(test_algo_2nd_deriv_3x3);
