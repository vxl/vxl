// This is tbl/vepl2/tests/vepl2_test_dilate_disk.cxx
#include "test_driver.h"
//:
// \file
//  Test of the vepl2_dilate_disk function.
//
// \author Peter Vanroose, K.U.Leuven, ESAT/PSI
// \date   7 October 2002, from vepl/tests

#include <vepl2/vepl2_dilate_disk.h>
#include <vcl_iostream.h>
#include <vcl_string.h>

int vepl2_test_dilate_disk()
{
  vcl_cout << "Starting vepl2_dilate_disk tests\n"
           << "Creating test and output images ...";
  vil2_image_view_base_sptr byte_img = CreateTest8bitImage(32,32),  byte_ori = CreateTest8bitImage(32,32);
  vil2_image_view_base_sptr shrt_img = CreateTest16bitImage(32,32), shrt_ori = CreateTest16bitImage(32,32);
  vil2_image_view_base_sptr int__img = CreateTest32bitImage(32,32), int__ori = CreateTest32bitImage(32,32);
  vil2_image_view_base_sptr flot_img = CreateTestfloatImage(32,32), flot_ori = CreateTestfloatImage(32,32);
  vil2_image_view_base_sptr dble_img = CreateTestdoubleImage(32,32),dble_ori = CreateTestdoubleImage(32,32);
  vil2_image_view_base_sptr colr_img = CreateTest3planeImage(32,32),colr_ori = CreateTest3planeImage(32,32);
  vcl_cout << " done\n";

  vcl_string m = "vepl2_dilate_disk";
#define args ,5
  ONE_TEST(vepl2_dilate_disk,byte_img,byte_ori,unsigned char,70355,m+"_byte",args);
  ONE_TEST(vepl2_dilate_disk,shrt_img,shrt_ori,unsigned short,17565395,m+"_short",args);
  ONE_TEST(vepl2_dilate_disk,int__img,int__ori,unsigned int,17565395,m+"_int",args);
  ONE_TEST(vepl2_dilate_disk,flot_img,flot_ori,float,338,m+"_float",args);
  ONE_TEST(vepl2_dilate_disk,dble_img,dble_ori,double,338,m+"_double",args);
  ONE_TEST(vepl2_dilate_disk,colr_img,colr_ori,vil2_rgb<unsigned char>,75965,m+"_colour",args);

  return 0;
}

TESTMAIN(vepl2_test_dilate_disk);
