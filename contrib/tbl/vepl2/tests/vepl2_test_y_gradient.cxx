// This is tbl/vepl2/tests/vepl2_test_y_gradient.cxx

//:
// \file
//  Test of the vepl2_y_gradient function.
//
// \author Peter Vanroose, K.U.Leuven, ESAT/PSI
// \date   7 October 2002, from vepl/tests

#include <vepl2/vepl2_y_gradient.h>
#include <vcl_iostream.h>
#include <vcl_string.h>
#include "test_driver.h"

int vepl2_test_y_gradient() {
  vcl_cout << "Starting vepl2_y_gradient tests\n";
  vcl_cout << "Creating test and output images ...";
  vil2_image_view_base_sptr byte_img = CreateTest8bitImage(32,32),  byte_ori = CreateTest8bitImage(32,32);
  vil2_image_view_base_sptr shrt_img = CreateTest16bitImage(32,32), shrt_ori = CreateTest16bitImage(32,32);
  vil2_image_view_base_sptr flot_img = CreateTestfloatImage(32,32), flot_ori = CreateTestfloatImage(32,32);
  vil2_image_view_base_sptr colr_img = CreateTest3planeImage(32,32),colr_ori = CreateTest3planeImage(32,32);
  vcl_cout << " done\n";

  vcl_string m = "vepl2_y_gradient";
#define args
  ONE_TEST(vepl2_y_gradient,byte_img,byte_ori,unsigned char,214,m+"_byte",args);
  ONE_TEST(vepl2_y_gradient,shrt_img,shrt_ori,unsigned short,65494,m+"_short",args);
  ONE_TEST(vepl2_y_gradient,flot_img,flot_ori,float,282,m+"_float",args);
  ONE_TEST(vepl2_y_gradient,colr_img,colr_ori,vil_rgb,134,m+"_colour",args);

  return 0;
}

TESTMAIN(vepl2_test_y_gradient);
