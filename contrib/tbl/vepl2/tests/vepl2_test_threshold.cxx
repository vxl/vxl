// This is tbl/vepl2/tests/vepl2_test_threshold.cxx
#include "test_driver.h"
//:
// \file
//  Test of the vepl2_threshold function.
//
// \author Peter Vanroose, K.U.Leuven, ESAT/PSI
// \date   7 October 2002, from vepl/tests

#include <vepl2/vepl2_threshold.h>
#include <vcl_iostream.h>
#include <vcl_string.h>

int vepl2_test_threshold()
{
  vcl_cout << "Starting vepl2_threshold tests\n"
           << "Creating test and output images ...";
  vil2_image_view_base_sptr byte_img = CreateTest8bitImage(32,32),  byte_ori = CreateTest8bitImage(32,32);
  vil2_image_view_base_sptr shrt_img = CreateTest16bitImage(32,32), shrt_ori = CreateTest16bitImage(32,32);
  vil2_image_view_base_sptr int__img = CreateTest32bitImage(32,32), int__ori = CreateTest32bitImage(32,32);
  vil2_image_view_base_sptr flot_img = CreateTestfloatImage(32,32), flot_ori = CreateTestfloatImage(32,32);
  vil2_image_view_base_sptr dble_img = CreateTestdoubleImage(32,32),dble_ori = CreateTestdoubleImage(32,32);
  vil2_image_view_base_sptr colr_img = CreateTest3planeImage(32,32),colr_ori = CreateTest3planeImage(32,32);
  vcl_cout << " done\n";

  vcl_string m = "vepl2_threshold";
#define args ,128
  ONE_TEST(vepl2_threshold,byte_img,byte_ori,unsigned char,84908,m+"_byte",args);
  ONE_TEST(vepl2_threshold,shrt_img,shrt_ori,unsigned short,21757868,m+"_short",args);
  ONE_TEST(vepl2_threshold,int__img,int__ori,unsigned int,21757868,m+"_int",args);
  ONE_TEST(vepl2_threshold,flot_img,flot_ori,float,903,m+"_float",args);
  ONE_TEST(vepl2_threshold,dble_img,dble_ori,double,903,m+"_double",args);
  ONE_TEST(vepl2_threshold,colr_img,colr_ori,vil2_rgb<unsigned char>,101528,m+"_colour",args);

  return 0;
}

TESTMAIN(vepl2_test_threshold);
