// This is tbl/vepl2/tests/vepl2_test_sobel.cxx
#include "test_driver.h"
//:
// \file
//  Test of the vepl2_sobel function.
//
// \author Peter Vanroose, K.U.Leuven, ESAT/PSI
// \date   7 October 2002, from vepl/tests

#include <vepl2/vepl2_sobel.h>
#include <vcl_iostream.h>
#include <vcl_string.h>

int vepl2_test_sobel()
{
  vcl_cout << "Starting vepl2_sobel tests\n"
           << "Creating test and output images ...";
  vil2_image_view_base_sptr byte_img = CreateTest8bitImage(32,32),  byte_ori = CreateTest8bitImage(32,32);
  vil2_image_view_base_sptr shrt_img = CreateTest16bitImage(32,32), shrt_ori = CreateTest16bitImage(32,32);
  vil2_image_view_base_sptr int__img = CreateTest32bitImage(32,32), int__ori = CreateTest32bitImage(32,32);
  vil2_image_view_base_sptr flot_img = CreateTestfloatImage(32,32), flot_ori = CreateTestfloatImage(32,32);
  vil2_image_view_base_sptr dble_img = CreateTestdoubleImage(32,32),dble_ori = CreateTestdoubleImage(32,32);
  vil2_image_view_base_sptr colr_img = CreateTest3planeImage(32,32),colr_ori = CreateTest3planeImage(32,32);
  vcl_cout << " done\n";

  vcl_string m = "vepl2_sobel";
#define args
  ONE_TEST(vepl2_sobel,byte_img,byte_ori,unsigned char,95634,m+"_byte",args);
  ONE_TEST(vepl2_sobel,shrt_img,shrt_ori,unsigned short,24640914,m+"_short",args);
  ONE_TEST(vepl2_sobel,int__img,int__ori,unsigned int,26738762,m+"_int",args);
  ONE_TEST(vepl2_sobel,flot_img,flot_ori,float,301,m+"_float",args);
  ONE_TEST(vepl2_sobel,dble_img,dble_ori,double,301,m+"_double",args);
  ONE_TEST(vepl2_sobel,colr_img,colr_ori,vil2_rgb<unsigned char>,186894,m+"_colour",args);

  return 0;
}

TESTMAIN(vepl2_test_sobel);
