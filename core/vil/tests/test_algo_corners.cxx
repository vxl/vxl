// This is core/vil/tests/test_binary_dilate.cxx
#include <testlib/testlib_test.h>
#include <vcl_iostream.h>
#include <vil/algo/vil_corners.h>
#include <vil/vil_crop.h>
#include <vxl_config.h>
#include <vil/algo/vil_find_peaks.h>
#include <vil/vil_print.h>

static void test_algo_corners_byte_float()
{
  vcl_cout << "***************************\n"
           << " Testing vil_corners\n"
           << "***************************\n";

  vcl_cout<<"Testing white square on black background"<<vcl_endl;
  vil_image_view<vxl_byte> image0;
  image0.set_size(20,20);
  image0.fill(0);

  // Put a square in the centre
  vil_crop(image0,6,8,6,8).fill(vxl_byte(100));

  vil_image_view<float> corner_im;
  vil_corners(image0,corner_im);

  TEST("Correct size",corner_im.ni(),image0.ni());

  TEST("Peak just inside corner",vil_is_peak_3x3(&corner_im(7,7),corner_im.istep(),corner_im.jstep()),true);
  TEST("Peak just inside corner",vil_is_peak_3x3(&corner_im(7,12),corner_im.istep(),corner_im.jstep()),true);
  TEST("Peak just inside corner",vil_is_peak_3x3(&corner_im(12,7),corner_im.istep(),corner_im.jstep()),true);
  TEST("Peak just inside corner",vil_is_peak_3x3(&corner_im(12,12),corner_im.istep(),corner_im.jstep()),true);


  vcl_cout<<"Testing black square on white background"<<vcl_endl;
  image0.fill(100);

  // Put a square in the centre
  vil_crop(image0,6,8,6,8).fill(vxl_byte(0));

  vil_corners(image0,corner_im);

  TEST("Correct size",corner_im.ni(),image0.ni());

  TEST("Peak just inside corner",vil_is_peak_3x3(&corner_im(7,7),corner_im.istep(),corner_im.jstep()),true);
  TEST("Peak just inside corner",vil_is_peak_3x3(&corner_im(7,12),corner_im.istep(),corner_im.jstep()),true);
  TEST("Peak just inside corner",vil_is_peak_3x3(&corner_im(12,7),corner_im.istep(),corner_im.jstep()),true);
  TEST("Peak just inside corner",vil_is_peak_3x3(&corner_im(12,12),corner_im.istep(),corner_im.jstep()),true);
}

MAIN( test_algo_corners )
{
  START( "Testing Corners" );

  test_algo_corners_byte_float();

  SUMMARY();
}
