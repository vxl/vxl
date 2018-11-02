// This is tbl/vepl/tests/vepl_test_threshold.cxx
#include <iostream>
#include <string>
#include "test_driver.h"
//:
// \file
//  Test of the vepl_threshold function.
//
// \author Peter Vanroose, K.U.Leuven, ESAT/PSI
// \date   7 October 2002, from vepl1/tests

#include <vepl/vepl_threshold.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vxl_config.h> // for vxl_byte

int vepl_test_threshold()
{
  std::cout << "Starting vepl_threshold tests\n"
           << "Creating test and output images ...";
  vil_image_resource_sptr byte_img = CreateTest8bitImage(32,32),  byte_ori = CreateTest8bitImage(32,32);
  vil_image_resource_sptr shrt_img = CreateTest16bitImage(32,32), shrt_ori = CreateTest16bitImage(32,32);
  vil_image_resource_sptr int__img = CreateTest32bitImage(32,32), int__ori = CreateTest32bitImage(32,32);
  vil_image_resource_sptr flot_img = CreateTestfloatImage(32,32), flot_ori = CreateTestfloatImage(32,32);
  vil_image_resource_sptr dble_img = CreateTestdoubleImage(32,32),dble_ori = CreateTestdoubleImage(32,32);
  vil_image_resource_sptr colr_img = CreateTest24bitImage(32,32), colr_ori = CreateTest24bitImage(32,32);
  vil_image_resource_sptr colp_img = CreateTest3planeImage(32,32),colp_ori = CreateTest3planeImage(32,32);
  std::cout << " done\n";

  std::string m = "vepl_threshold";
#define args ,128
  ONE_TEST(vepl_threshold,byte_img,byte_ori,vxl_byte,84908,m+"_byte",args);
  ONE_TEST(vepl_threshold,shrt_img,shrt_ori,vxl_uint_16,21757868,m+"_short",args);
//ONE_TEST(vepl_threshold,int__img,int__ori,vxl_uint_32,21757868,m+"_int",args);
  ONE_TEST(vepl_threshold,flot_img,flot_ori,float,903,m+"_float",args);
  ONE_TEST(vepl_threshold,dble_img,dble_ori,double,903,m+"_double",args);
  ONE_TEST(vepl_threshold,colr_img,colr_ori,vil_rgb<vxl_byte>,101528,m+"_colour",args);
  ONE_TEST(vepl_threshold,colp_img,colp_ori,vxl_byte,101528,m+"_planar",args);

  return 0;
}

TESTMAIN(vepl_test_threshold);
