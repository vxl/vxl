// This is tbl/vepl2/tests/vepl2_test_monadic.cxx

//:
// \file
//  Test of the vepl2_monadic_xxx functions.
//
// \author Peter Vanroose, K.U.Leuven, ESAT/PSI
// \date   7 October 2002, from vepl/tests

#include <vepl2/vepl2_monadic.h>
#include <vcl_iostream.h>
#include <vcl_string.h>
#include "test_driver.h"

int vepl2_test_monadic() {
  vcl_cout << "Starting vepl2_monadic tests\n";
  vcl_cout << "Creating test and output images ...";
  vil2_image_view_base_sptr byte_img = CreateTest8bitImage(32,32),  byte_ori = CreateTest8bitImage(32,32);
  vil2_image_view_base_sptr shrt_img = CreateTest16bitImage(32,32), shrt_ori = CreateTest16bitImage(32,32);
  vil2_image_view_base_sptr flot_img = CreateTestfloatImage(32,32), flot_ori = CreateTestfloatImage(32,32);
  vil2_image_view_base_sptr colr_img = CreateTest3planeImage(32,32),colr_ori = CreateTest3planeImage(32,32);
  vcl_cout << " done\n";

  vcl_string m = "vepl2_monadic";
#define args
  ONE_TEST(vepl2_monadic_abs,byte_img,byte_ori,unsigned char,0,m+"_abs_byte",args);
  ONE_TEST(vepl2_monadic_abs,shrt_img,shrt_ori,unsigned short,0,m+"_abs_short",args);
  ONE_TEST(vepl2_monadic_abs,flot_img,flot_ori,float,276,m+"_abs_float",args);
  ONE_TEST(vepl2_monadic_abs,colr_img,colr_ori,vil_rgb,0,m+"_abs_colour",args);

  ONE_TEST(vepl2_monadic_sqr,byte_img,byte_ori,unsigned char,82410,m+"_sqr_byte",args);
  ONE_TEST(vepl2_monadic_sqr,shrt_img,shrt_ori,unsigned short,21755370,m+"_sqr_short",args);
  ONE_TEST(vepl2_monadic_sqr,flot_img,flot_ori,float,282,m+"_sqr_float",args);
#undef args
#define args ,10,20
  ONE_TEST(vepl2_monadic_shear,byte_img,byte_ori,unsigned char,127886,m+"_shear_byte",args);
  ONE_TEST(vepl2_monadic_shear,shrt_img,shrt_ori,unsigned short,21071502,m+"_shear_short",args);
  ONE_TEST(vepl2_monadic_shear,flot_img,flot_ori,float,180027,m+"_shear_float",args);

  return 0;
}

TESTMAIN(vepl2_test_monadic);
