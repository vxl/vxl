// This is tbl/vepl2/tests/vepl2_test_dyadic.cxx
#include "test_driver.h"
//:
// \file
//  Test of the vepl2_dyadic_xxx functions.
//
// \author Peter Vanroose, K.U.Leuven, ESAT/PSI
// \date   7 October 2002, from vepl/tests

#include <vepl2/vepl2_dyadic.h>
#include <vcl_iostream.h>
#include <vcl_string.h>
#include <vxl_config.h> // for vxl_byte

int vepl2_test_dyadic()
{
  vcl_cout << "Starting vepl2_dyadic tests\n"
           << "Creating test and output images ...";
  vil_image_resource_sptr byte_img = CreateTest8bitImage(32,32),  byte_ori = CreateTest8bitImage(32,32);
  vil_image_resource_sptr shrt_img = CreateTest16bitImage(32,32), shrt_ori = CreateTest16bitImage(32,32);
  vil_image_resource_sptr int__img = CreateTest32bitImage(32,32), int__ori = CreateTest32bitImage(32,32);
  vil_image_resource_sptr flot_img = CreateTestfloatImage(32,32), flot_ori = CreateTestfloatImage(32,32);
  vil_image_resource_sptr dble_img = CreateTestdoubleImage(32,32),dble_ori = CreateTestdoubleImage(32,32);
  vil_image_resource_sptr colr_img = CreateTest24bitImage(32,32), colr_ori = CreateTest24bitImage(32,32);
  vil_image_resource_sptr colp_img = CreateTest3planeImage(32,32),colp_ori = CreateTest3planeImage(32,32);
  vcl_cout << " done\n";

  vcl_string m = "vepl2_dyadic";
#undef ONE_TEST
#define ONE_TEST(x,i,r,T,v,m) { vcl_cout << "Starting "<<m<<" test\n"; x(r,i); difference(i,r,v,m); }
#define args
  ONE_TEST(vepl2_dyadic_sum,byte_img,byte_ori,vxl_byte,2778,m+"_sum_byte");
  ONE_TEST(vepl2_dyadic_sum,shrt_img,shrt_ori,vxl_uint_16,2778,m+"_sum_short");
  ONE_TEST(vepl2_dyadic_sum,int__img,int__ori,vxl_uint_32,21757966,m+"_sum_int");
  ONE_TEST(vepl2_dyadic_sum,flot_img,flot_ori,float,278,m+"_sum_float");
  ONE_TEST(vepl2_dyadic_sum,dble_img,dble_ori,double,278,m+"_sum_double");
  ONE_TEST(vepl2_dyadic_sum,colr_img,colr_ori,vil_rgb<vxl_byte>,13950,m+"_sum_colour");
  ONE_TEST(vepl2_dyadic_sum,colp_img,colp_ori,vxl_byte,13950,m+"_sum_planar");

  ONE_TEST(vepl2_dyadic_dif,byte_img,byte_ori,vxl_byte,0,m+"_dif_byte");
  ONE_TEST(vepl2_dyadic_dif,shrt_img,shrt_ori,vxl_uint_16,0,m+"_dif_short");
  ONE_TEST(vepl2_dyadic_dif,int__img,int__ori,vxl_uint_32,0,m+"_dif_int");
  ONE_TEST(vepl2_dyadic_dif,flot_img,flot_ori,float,0,m+"_dif_float");
  ONE_TEST(vepl2_dyadic_dif,dble_img,dble_ori,double,0,m+"_dif_double");
  ONE_TEST(vepl2_dyadic_dif,colr_img,colr_ori,vil_rgb<vxl_byte>,0,m+"_dif_colour");
  ONE_TEST(vepl2_dyadic_dif,colp_img,colp_ori,vxl_byte,0,m+"_dif_planar");

  ONE_TEST(vepl2_dyadic_min,byte_img,byte_ori,vxl_byte,0,m+"min__byte");
  ONE_TEST(vepl2_dyadic_min,shrt_img,shrt_ori,vxl_uint_16,0,m+"min__short");
  ONE_TEST(vepl2_dyadic_min,int__img,int__ori,vxl_uint_32,0,m+"min__int");
  ONE_TEST(vepl2_dyadic_min,flot_img,flot_ori,float,0,m+"min__float");
  ONE_TEST(vepl2_dyadic_min,dble_img,dble_ori,double,0,m+"min__double");

  ONE_TEST(vepl2_dyadic_max,byte_img,byte_ori,vxl_byte,0,m+"_max_byte");
  ONE_TEST(vepl2_dyadic_max,shrt_img,shrt_ori,vxl_uint_16,0,m+"_max_short");
  ONE_TEST(vepl2_dyadic_max,int__img,int__ori,vxl_uint_32,0,m+"_max_int");
  ONE_TEST(vepl2_dyadic_max,flot_img,flot_ori,float,0,m+"_max_float");
  ONE_TEST(vepl2_dyadic_max,dble_img,dble_ori,double,0,m+"_max_double");

  return 0;
}

TESTMAIN(vepl2_test_dyadic);
