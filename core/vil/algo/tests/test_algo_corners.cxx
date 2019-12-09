// This is core/vil/algo/tests/test_algo_corners.cxx
#include <iostream>
#include "testlib/testlib_test.h"
#ifdef _MSC_VER
#  include "vcl_msvc_warnings.h"
#endif
#include <vil/algo/vil_corners.h>
#include "vil/vil_crop.h"
#include "vxl_config.h"
#include <vil/algo/vil_find_peaks.h>

static void
test_algo_corners_byte_float()
{
  std::cout << "*********************\n"
            << " Testing vil_corners\n"
            << "*********************\n";

  vil_image_view<vxl_byte> image0;
  image0.set_size(20, 20);

  std::cout << "Testing white square on black background\n";
  image0.fill(0);
  // Put a white square in the centre
  vil_crop(image0, 6, 8, 6, 8).fill(vxl_byte(100));

  vil_image_view<float> corner_im;
  vil_corners(image0, corner_im);

  TEST("Correct size", corner_im.ni(), image0.ni());

  TEST("Peak just inside corner", vil_is_peak_3x3(&corner_im(7, 7), corner_im.istep(), corner_im.jstep()), true);
  TEST("Peak just inside corner", vil_is_peak_3x3(&corner_im(7, 12), corner_im.istep(), corner_im.jstep()), true);
  TEST("Peak just inside corner", vil_is_peak_3x3(&corner_im(12, 7), corner_im.istep(), corner_im.jstep()), true);
  TEST("Peak just inside corner", vil_is_peak_3x3(&corner_im(12, 12), corner_im.istep(), corner_im.jstep()), true);


  std::cout << "Testing black square on white background\n";
  image0.fill(100);
  // Put a black square in the centre
  vil_crop(image0, 6, 8, 6, 8).fill(vxl_byte(0));

  vil_corners(image0, corner_im);

  TEST("Correct size", corner_im.ni(), image0.ni());

  TEST("Peak just inside corner", vil_is_peak_3x3(&corner_im(7, 7), corner_im.istep(), corner_im.jstep()), true);
  TEST("Peak just inside corner", vil_is_peak_3x3(&corner_im(7, 12), corner_im.istep(), corner_im.jstep()), true);
  TEST("Peak just inside corner", vil_is_peak_3x3(&corner_im(12, 7), corner_im.istep(), corner_im.jstep()), true);
  TEST("Peak just inside corner", vil_is_peak_3x3(&corner_im(12, 12), corner_im.istep(), corner_im.jstep()), true);
}

static void
test_algo_corners()
{
  test_algo_corners_byte_float();
}

TESTMAIN(test_algo_corners);
