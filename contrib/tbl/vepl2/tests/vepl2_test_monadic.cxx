// This is tbl/vepl2/tests/vepl2_test_monadic.cxx
#include "test_driver.h"
//:
// \file
//  Test of the vepl2_monadic_xxx functions.
//
// \author Peter Vanroose, K.U.Leuven, ESAT/PSI
// \date   7 October 2002, from vepl/tests

#include <vepl2/vepl2_monadic.h>
#include <vcl_iostream.h>
#include <vcl_string.h>
#include <vxl_config.h> // for vxl_byte

int vepl2_test_monadic()
{
  vcl_cout << "Starting vepl2_monadic tests\n"
           << "Creating test and output images ...";
  vil_image_resource_sptr byte_img = CreateTest8bitImage(32,32),  byte_ori = CreateTest8bitImage(32,32);
  vil_image_resource_sptr shrt_img = CreateTest16bitImage(32,32), shrt_ori = CreateTest16bitImage(32,32);
  vil_image_resource_sptr int__img = CreateTest32bitImage(32,32), int__ori = CreateTest32bitImage(32,32);
  vil_image_resource_sptr flot_img = CreateTestfloatImage(32,32), flot_ori = CreateTestfloatImage(32,32);
  vil_image_resource_sptr dble_img = CreateTestdoubleImage(32,32),dble_ori = CreateTestdoubleImage(32,32);
  vil_image_resource_sptr colr_img = CreateTest24bitImage(32,32), colr_ori = CreateTest24bitImage(32,32);
  vil_image_resource_sptr colp_img = CreateTest3planeImage(32,32),colp_ori = CreateTest3planeImage(32,32);
  vcl_cout << " done\n";

  vcl_string m = "vepl2_monadic";
#define args
  ONE_TEST(vepl2_monadic_abs,byte_img,byte_ori,vxl_byte,0,m+"_abs_byte",args);
  ONE_TEST(vepl2_monadic_abs,shrt_img,shrt_ori,vxl_uint_16,0,m+"_abs_short",args);
  ONE_TEST(vepl2_monadic_abs,int__img,int__ori,vxl_uint_32,0,m+"_abs_int",args);
  ONE_TEST(vepl2_monadic_abs,flot_img,flot_ori,float,276,m+"_abs_float",args);
  ONE_TEST(vepl2_monadic_abs,dble_img,dble_ori,double,276,m+"_abs_double",args);
  ONE_TEST(vepl2_monadic_abs,colr_img,colr_ori,vil_rgb<vxl_byte>,0,m+"_abs_colour",args);
  ONE_TEST(vepl2_monadic_abs,colp_img,colp_ori,vxl_byte,0,m+"_abs_planar",args);

  ONE_TEST(vepl2_monadic_sqr,byte_img,byte_ori,vxl_byte,82410,m+"_sqr_byte",args);
  ONE_TEST(vepl2_monadic_sqr,shrt_img,shrt_ori,vxl_uint_16,21755370,m+"_sqr_short",args);
  ONE_TEST(vepl2_monadic_sqr,int__img,int__ori,vxl_uint_32,202896874,m+"_sqr_int",args);
  ONE_TEST(vepl2_monadic_sqr,flot_img,flot_ori,float,282,m+"_sqr_float",args);
  ONE_TEST(vepl2_monadic_sqr,dble_img,dble_ori,double,282,m+"_sqr_double",args);

  FUZ_TEST(vepl2_monadic_sqrt,flot_img,flot_ori,float,203,m+"_sqrt_float",args);
  FUZ_TEST(vepl2_monadic_sqrt,dble_img,dble_ori,double,203,m+"_sqrt_double",args);
#undef args
#define args ,10,20
  ONE_TEST(vepl2_monadic_shear,byte_img,byte_ori,vxl_byte,127886,m+"_shear_byte",args);
  ONE_TEST(vepl2_monadic_shear,shrt_img,shrt_ori,vxl_uint_16,21071502,m+"_shear_short",args);
  ONE_TEST(vepl2_monadic_shear,int__img,int__ori,vxl_uint_32,413581354,m+"_shear_int",args);
  ONE_TEST(vepl2_monadic_shear,flot_img,flot_ori,float,180027,m+"_shear_float",args);
  ONE_TEST(vepl2_monadic_shear,dble_img,dble_ori,double,180027,m+"_shear_double",args);

  return 0;
}

TESTMAIN(vepl2_test_monadic);
