// This is core/vil/algo/tests/test_algo_colour_space.cxx
#include <testlib/testlib_test.h>
#include <vil/algo/vil_colour_space.h>
// not used? #include <vcl_iostream.h>
#include <vcl_cmath.h>

static void test_algo_colour_space()
{
  double red_rgb[3]   = {1.0, 0.0, 0.0};
  double green_rgb[3] = {0.0, 1.0, 0.0};
  double blue_rgb[3]  = {0.0, 0.0, 1.0};

  double color1[3], color2[3];
  // test on red
  vil_colour_space_RGB_to_YUV(red_rgb, color1);
  vil_colour_space_YUV_to_RGB(color1, color2);
  TEST("Test RGB/YUV on red",
       vcl_abs(red_rgb[0]-color2[0])<1e-12 &&
       vcl_abs(red_rgb[1]-color2[1])<1e-12 &&
       vcl_abs(red_rgb[2]-color2[2])<1e-12, true);

  // test on green
  vil_colour_space_RGB_to_YUV(green_rgb, color1);
  vil_colour_space_YUV_to_RGB(color1, color2);
  TEST("Test RGB/YUV on green",
       vcl_abs(green_rgb[0]-color2[0])<1e-12 &&
       vcl_abs(green_rgb[1]-color2[1])<1e-12 &&
       vcl_abs(green_rgb[2]-color2[2])<1e-12, true);

  // test on blue
  vil_colour_space_RGB_to_YUV(blue_rgb, color1);
  vil_colour_space_YUV_to_RGB(color1, color2);
  TEST("Test RGB/YUV on blue",
       vcl_abs(blue_rgb[0]-color2[0])<1e-12 &&
       vcl_abs(blue_rgb[1]-color2[1])<1e-12 &&
       vcl_abs(blue_rgb[2]-color2[2])<1e-12, true);
}

TESTMAIN(test_algo_colour_space);
