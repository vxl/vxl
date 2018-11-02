// This is tbl/vepl/tests/vepl_test_moment.cxx

//:
// \file
//  Test of the vepl_moment function.
//
// \author Peter Vanroose, K.U.Leuven, ESAT/PSI
// \date   7 October 2002, from vepl1/tests

#include <iostream>
#include <string>
#include <vepl/vepl_moment.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include "test_driver.h"
#include <vxl_config.h> // for vxl_byte

int vepl_test_moment()
{
  std::cout << "Starting vepl_moment tests\n"
           << "Creating test and output images ...";
  vil_image_resource_sptr byte_img = CreateTest8bitImage(32,32),  byte_ori = CreateTest8bitImage(32,32);
  vil_image_resource_sptr shrt_img = CreateTest16bitImage(32,32), shrt_ori = CreateTest16bitImage(32,32);
  vil_image_resource_sptr int__img = CreateTest32bitImage(32,32), int__ori = CreateTest32bitImage(32,32);
  vil_image_resource_sptr flot_img = CreateTestfloatImage(32,32), flot_ori = CreateTestfloatImage(32,32);
  vil_image_resource_sptr dble_img = CreateTestdoubleImage(32,32),dble_ori = CreateTestdoubleImage(32,32);
  std::cout << " done\n";

  std::string m = "vepl_moment";
#define args ,2
  FUZ_TEST(vepl_moment,byte_img,byte_ori,vxl_byte,59985,m+"_byte",args);
  FUZ_TEST(vepl_moment,shrt_img,shrt_ori,vxl_uint_16,23424263,m+"_short",args);
  ONE_TEST(vepl_moment,int__img,int__ori,vxl_uint_32,2658594499U,m+"_int",args);
  ONE_TEST(vepl_moment,flot_img,flot_ori,float,283,m+"_float",args);
  ONE_TEST(vepl_moment,dble_img,dble_ori,double,283,m+"_double",args);

  return 0;
}

TESTMAIN(vepl_test_moment);
