// This is tbl/vepl2/tests/vepl2_test_dyadic.cxx

//:
// \file
//  Test of the vepl2_dyadic_xxx functions.
//
// \author Peter Vanroose, K.U.Leuven, ESAT/PSI
// \date   7 October 2002, from vepl/tests

#include <vepl2/vepl2_dyadic.h>
#include <vcl_iostream.h>
#include <vcl_string.h>
#include "test_driver.h"

int vepl2_test_dyadic() {
  vcl_cout << "Starting vepl2_dyadic tests\n";
  vcl_cout << "Creating test and output images ...";
  vil2_image_view_base_sptr byte_img = CreateTest8bitImage(32,32),  byte_ori = CreateTest8bitImage(32,32);
  vil2_image_view_base_sptr shrt_img = CreateTest16bitImage(32,32), shrt_ori = CreateTest16bitImage(32,32);
  vil2_image_view_base_sptr flot_img = CreateTestfloatImage(32,32), flot_ori = CreateTestfloatImage(32,32);
  vil2_image_view_base_sptr colr_img = CreateTest3planeImage(32,32),colr_ori = CreateTest3planeImage(32,32);
  vcl_cout << " done\n";

  vcl_string m = "vepl2_dyadic";
#undef ONE_TEST
#define ONE_TEST(x,i,r,T,v,m) { vcl_cout << "Starting "<<m<<" test\n"; x(r,*i); difference(i,r,v,m); }
#define args
  ONE_TEST(vepl2_dyadic_sum,byte_img,byte_ori,unsigned char,2778,m+"_sum_byte");
  ONE_TEST(vepl2_dyadic_sum,shrt_img,shrt_ori,unsigned short,2778,m+"_sum_short");
  ONE_TEST(vepl2_dyadic_sum,flot_img,flot_ori,float,278,m+"_sum_float");
  ONE_TEST(vepl2_dyadic_sum,colr_img,colr_ori,vil_rgb,13950,m+"_sum_colour");

  ONE_TEST(vepl2_dyadic_dif,byte_img,byte_ori,unsigned char,0,m+"_dif_byte");
  ONE_TEST(vepl2_dyadic_dif,shrt_img,shrt_ori,unsigned short,0,m+"_dif_short");
  ONE_TEST(vepl2_dyadic_dif,flot_img,flot_ori,float,0,m+"_dif_float");
  ONE_TEST(vepl2_dyadic_dif,colr_img,colr_ori,vil_rgb,0,m+"_dif_colour");

  ONE_TEST(vepl2_dyadic_min,byte_img,byte_ori,unsigned char,0,m+"min__byte");
  ONE_TEST(vepl2_dyadic_min,shrt_img,shrt_ori,unsigned short,0,m+"min__short");
  ONE_TEST(vepl2_dyadic_min,flot_img,flot_ori,float,0,m+"min__float");

  ONE_TEST(vepl2_dyadic_max,byte_img,byte_ori,unsigned char,0,m+"_max_byte");
  ONE_TEST(vepl2_dyadic_max,shrt_img,shrt_ori,unsigned short,0,m+"_max_short");
  ONE_TEST(vepl2_dyadic_max,flot_img,flot_ori,float,0,m+"_max_float");

  return 0;
}

TESTMAIN(vepl2_test_dyadic);
