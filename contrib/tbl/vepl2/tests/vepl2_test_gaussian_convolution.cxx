// This is tbl/vepl2/tests/vepl2_test_gaussian_convolution.cxx

//:
// \file
//  Test of the vepl2_gaussian_convolution function.
//
// \author Peter Vanroose, K.U.Leuven, ESAT/PSI
// \date   7 October 2002, from vepl/tests

#include <vepl2/vepl2_gaussian_convolution.h>
#include <vcl_iostream.h>
#include <vcl_string.h>
#include "test_driver.h"

int vepl2_test_gaussian_convolution() {
  vcl_cout << "Starting vepl2_gaussian_convolution tests\n"
           << "Creating test and output images ...";
  vil2_image_view_base_sptr byte_img = CreateTest8bitImage(32,32),  byte_ori = CreateTest8bitImage(32,32);
  vil2_image_view_base_sptr shrt_img = CreateTest16bitImage(32,32), shrt_ori = CreateTest16bitImage(32,32);
  vil2_image_view_base_sptr int__img = CreateTest32bitImage(32,32), int__ori = CreateTest32bitImage(32,32);
  vil2_image_view_base_sptr flot_img = CreateTestfloatImage(32,32), flot_ori = CreateTestfloatImage(32,32);
  vil2_image_view_base_sptr dble_img = CreateTestdoubleImage(32,32),dble_ori = CreateTestdoubleImage(32,32);
  vcl_cout << " done\n";

  vcl_string m = "vepl2_gaussian_convolution";
#define args ,9
  ONE_TEST(vepl2_gaussian_convolution,byte_img,byte_ori,unsigned char,84375,m+"_byte",args);
  ONE_TEST(vepl2_gaussian_convolution,shrt_img,shrt_ori,unsigned short,22181967,m+"_short",args);
  ONE_TEST(vepl2_gaussian_convolution,int__img,int__ori,unsigned int,22181967,m+"_int",args);
  ONE_TEST(vepl2_gaussian_convolution,flot_img,flot_ori,float,223,m+"_float",args);
  ONE_TEST(vepl2_gaussian_convolution,dble_img,dble_ori,double,223,m+"_double",args);

  return 0;
}

TESTMAIN(vepl2_test_gaussian_convolution);
