// This is tbl/vepl/tests/vepl_test_sobel.cxx
#include <iostream>
#include <string>
#include "test_driver.h"
//:
// \file
//  Test of the vepl_sobel function.
//
// \author Peter Vanroose, K.U.Leuven, ESAT/PSI
// \date   7 October 2002, from vepl1/tests

#include <vepl/vepl_sobel.h>
#include <vcl_compiler.h>
#include <vxl_config.h> // for vxl_byte

int vepl_test_sobel()
{
  std::cout << "Starting vepl_sobel tests\n"
           << "Creating test and output images ...";
  vil_image_resource_sptr byte_img = CreateTest8bitImage(32,32),  byte_ori = CreateTest8bitImage(32,32);
  vil_image_resource_sptr shrt_img = CreateTest16bitImage(32,32), shrt_ori = CreateTest16bitImage(32,32);
  vil_image_resource_sptr int__img = CreateTest32bitImage(32,32), int__ori = CreateTest32bitImage(32,32);
  vil_image_resource_sptr flot_img = CreateTestfloatImage(32,32), flot_ori = CreateTestfloatImage(32,32);
  vil_image_resource_sptr dble_img = CreateTestdoubleImage(32,32),dble_ori = CreateTestdoubleImage(32,32);
  vil_image_resource_sptr colr_img = CreateTest24bitImage(32,32), colr_ori = CreateTest24bitImage(32,32);
  vil_image_resource_sptr colp_img = CreateTest3planeImage(32,32),colp_ori = CreateTest3planeImage(32,32);
  std::cout << " done\n";

  std::string m = "vepl_sobel";
#define args
  ONE_TEST(vepl_sobel,byte_img,byte_ori,vxl_byte,95634,m+"_byte",args);
  ONE_TEST(vepl_sobel,shrt_img,shrt_ori,vxl_uint_16,24640914,m+"_short",args);
  ONE_TEST(vepl_sobel,int__img,int__ori,vxl_uint_32,4193710,m+"_int",args);
  ONE_TEST(vepl_sobel,flot_img,flot_ori,float,301,m+"_float",args);
  ONE_TEST(vepl_sobel,dble_img,dble_ori,double,301,m+"_double",args);
  ONE_TEST(vepl_sobel,colr_img,colr_ori,vil_rgb<vxl_byte>,186894,m+"_colour",args);
  ONE_TEST(vepl_sobel,colp_img,colp_ori,vxl_byte,186894,m+"_planar",args);

  return 0;
}

TESTMAIN(vepl_test_sobel);
