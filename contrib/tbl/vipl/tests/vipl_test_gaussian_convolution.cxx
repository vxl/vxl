// This is tbl/vipl/tests/vipl_test_gaussian_convolution.cxx

//:
// \file
//  Test of the vipl_gaussian_convolution templated IP classes
//  vipl_gaussian_convolution<vil_image_view<T>,vil_image_view<T>,T,T>
//  for T in { vxl_byte, short, float }.
//
// \author Peter Vanroose, K.U.Leuven, ESAT/PSI
// \date   26 april 2002

#include <vil/vil_image_view.h>
#include <vipl/accessors/vipl_accessors_vil_image_view.h>
#include <vipl/vipl_gaussian_convolution.h>
#include "test_driver.h"
#include <vxl_config.h> // for vxl_byte

#define ALL_TESTS(x,m,p) \
  ONE_TEST(x,p,byte_img,byte_ori,vil_image_view<vxl_byte>,vxl_byte,10705,m+"_byte"); \
  ONE_TEST(x,p,shrt_img,shrt_ori,vil_image_view<short>,short,385,m+"_short"); \
  ONE_TEST(x,p,flot_img,flot_ori,vil_image_view<float>,float,2.19553,m+"_float")

int vipl_test_gaussian_convolution()
{
  vil_image_view<vxl_byte> byte_img = CreateTest8bitImage(32,32),  byte_ori = CreateTest8bitImage(32,32);
  vil_image_view<short> shrt_img = CreateTest16bitImage(32,32), shrt_ori = CreateTest16bitImage(32,32);
  vil_image_view<float> flot_img = CreateTestfloatImage(32,32), flot_ori = CreateTestfloatImage(32,32);

  vcl_string m = "vipl_gaussian_convolution";
  ALL_TESTS(vipl_gaussian_convolution,m,1.0);

  return 0;
}


TESTMAIN(vipl_test_gaussian_convolution);
