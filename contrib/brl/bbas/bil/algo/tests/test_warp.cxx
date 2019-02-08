#include <iostream>
#include <cmath>
#include <string>
#include <testlib/testlib_test.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <bil/algo/bil_warp.h>
#include <vil/vil_image_resource.h>
#include <vil/vil_image_view.h>
#include <vil/vil_new.h>
#include <vil/vil_load.h>
#include <vil/vil_save.h>
#include <vnl/vnl_matrix_fixed.h>

static void test_warp()
{
  size_t ni = 100, nj = 200, np = 3;
  vil_image_view<vxl_byte> input(ni, nj, np), warp;
  for (size_t j = 0; j < nj; ++j)
    for (size_t i = 0; i < ni; ++i)
      for (size_t p = 0; p < 3; ++p) {
        if ((i+j)%(p+1) == 0)
          input(i, j, p) = 255;
        else
          input(i, j, p) = 0;
      }
  vnl_matrix_fixed<double, 3, 3> H;
  H.fill(0.0);
  H[0][0] = 0.707;   H[0][1] = -0.707;   H[1][0] = 0.707;   H[1][1] = 0.707;
  H[2][2] = 1.0;
  bool clip_not_translate = false;
  bool good = bil_warp_image_bilinear(input, H, warp, clip_not_translate);
  good = good && warp.ni() == 212 && warp.nj() == 212;
  good = good && static_cast<unsigned>(warp(94,106,1))==237;
  TEST("warp bilinear", good, true);

  ni = 150; nj = 150;
  vil_image_view<float> finput(ni, nj), fwarp;
  for (size_t j = 0; j < nj; ++j)
    for (size_t i = 0; i < ni; ++i)
      finput(i, j) = i*j+100.0f;
  clip_not_translate = true;
  good = bil_warp_image_bilinear(finput, H, fwarp, clip_not_translate);
  float test_val = fwarp(105, 105);
  good = good && fwarp.ni() == 106 && fwarp.nj() == 212;
  good = good && test_val == 100.0;
  TEST("warp bilinear with clipping", good, true);
}

TESTMAIN(test_warp);
