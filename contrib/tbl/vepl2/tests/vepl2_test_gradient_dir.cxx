// This is tbl/vepl2/tests/vepl2_test_gradient_dir.cxx

//:
// \file
//  Test of the vepl2_gradient_dir function.
//
// \author Peter Vanroose, K.U.Leuven, ESAT/PSI
// \date   7 October 2002, from vepl/tests

#include <vepl2/vepl2_gradient_dir.h>
#include <vcl_iostream.h>
#include <vcl_string.h>
#include "test_driver.h"
#include <vxl_config.h> // for vxl_byte

int vepl2_test_gradient_dir() {
  vcl_cout << "Starting vepl2_gradient_dir tests\n"
           << "Creating test and output images ...";
  vil_image_resource_sptr byte_img = CreateTest8bitImage(32,32),  byte_ori = CreateTest8bitImage(32,32);
  vil_image_resource_sptr shrt_img = CreateTest16bitImage(32,32), shrt_ori = CreateTest16bitImage(32,32);
  vil_image_resource_sptr int__img = CreateTest32bitImage(32,32), int__ori = CreateTest32bitImage(32,32);
  vil_image_resource_sptr flot_img = CreateTestfloatImage(32,32), flot_ori = CreateTestfloatImage(32,32);
  vil_image_resource_sptr dble_img = CreateTestdoubleImage(32,32),dble_ori = CreateTestdoubleImage(32,32);
  vil_image_resource_sptr colr_img = CreateTest24bitImage(32,32), colr_ori = CreateTest24bitImage(32,32);
  vil_image_resource_sptr colp_img = CreateTest3planeImage(32,32),colp_ori = CreateTest3planeImage(32,32);
  vcl_cout << " done\n";

  vcl_string m = "vepl2_gradient_dir";
#define args
  ONE_TEST(vepl2_gradient_dir,byte_img,byte_ori,vxl_byte,105506,m+"_byte",args);
  ONE_TEST(vepl2_gradient_dir,shrt_img,shrt_ori,vxl_uint_16,27327266,m+"_short",args);
  ONE_TEST(vepl2_gradient_dir,int__img,int__ori,vxl_uint_32,21757788,m+"_int",args);
  ONE_TEST(vepl2_gradient_dir,flot_img,flot_ori,float,1058,m+"_float",args);
  ONE_TEST(vepl2_gradient_dir,dble_img,dble_ori,double,1058,m+"_double",args);
  ONE_TEST(vepl2_gradient_dir,colr_img,colr_ori,vil_rgb<vxl_byte>,123506,m+"_colour",args);
  ONE_TEST(vepl2_gradient_dir,colp_img,colp_ori,vxl_byte,123506,m+"_planar",args);

  return 0;
}

TESTMAIN(vepl2_test_gradient_dir);
